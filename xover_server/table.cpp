// Contains the main logic of game
//
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
#include "messagetypes.h"
#include "Table.h"

#include <random>

#ifdef _WIN32
#include <Windows.h>
#else
# include <sys/time.h>
#endif

int								xover::CDealerTable::FindPlayer					(const utility::string_t &name)				{
    int														idx								= 0;
    for (auto iter = Players.begin(); iter != Players.end(); iter++, idx++)
        if ( iter->Name == name )
            return idx;
    return -1;
}

// TODO: Right now, players must make sure to get in when the dealer is taking bets.
//       Change the logic to anticipate that a new player may come to the table at any point and wait until the next round.
bool							xover::CDealerTable::AddPlayer						(const CPlayer &player)						{
    pplx::extensibility::scoped_critical_section_t			lck								(ResponseLock);

    int														idx								= FindPlayer(player.Name);

    if ( idx > 0 )
        return false;

    Players				.push_back(player			);
    Responses			.push_back(TMessageWrapper());
    Pendingrefresh		.push_back(ST_None			);

    return true;
}

bool							xover::CDealerTable::RemovePlayer					(const utility::string_t &name)				{
    pplx::extensibility::scoped_critical_section_t			lck								(ResponseLock);

    std::vector<TMessageWrapper	>::const_iterator			evnts							= Responses			.begin();
    std::vector<EBJStatus		>::const_iterator			pends							= Pendingrefresh	.begin();

    for (auto iter = Players.begin(); iter != Players.end(); iter++, evnts++, pends++) {
        if ( iter->Name == name ) {
            Players			.erase(iter );
            Responses		.erase(evnts);
            Pendingrefresh	.erase(pends);
            return true;
        }
    }
    return false;
}


::nwol::error_t					xover::CDealerTable::Deal							()											{
    // Give everyone two cards.
    Players[0].Hand.RevealBoth							= false;
    CurrentPlayer										= 0;
    Betting												= false;

    for (size_t i = 0; i < Players.size(); i++) {
        Players[i].Hand.Clear();
        if ( i == 0 || Players[i].Hand.Bet > 0 ) {
            SCard													card							= Shoe.front(); 
			Shoe.pop();
            Players[i].Hand.Cards.push_back(card);
        }
    }
    for (size_t i = 0; i < Players.size(); i++)
        if ( i == 0 || Players[i].Hand.Bet > 0 ) {
            SCard													card							= Shoe.front(); 
			Shoe.pop();
            Players[i].Hand.AddCard(card);
        }

    pplx::extensibility::scoped_critical_section_t			lck								(ResponseLock);

    for (size_t player = CurrentPlayer+1; player < Players.size(); player++) {
        CurrentPlayer										= (int)player;

        if ( Players[player].Hand.Bet == 0 ) 
            continue;

        if ( Players[player].Hand.State == CBJHand::HR_Active ) {
			web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
			CBJPutResponse											putResponse						= {ST_YourTurn, jsonTable};
			web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
            Responses[player]->reply(web::http::status_codes::OK, jsonResponse);
            Responses[player].reset();
            break;
        }
    }

    for (size_t i = 1; i < Players.size(); i++)
        if ( ((int32_t)i) != CurrentPlayer ) {
            if (Responses[i]) {
				web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
				CBJPutResponse											putResponse						= {ST_Refresh, jsonTable};
				web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
                Responses[i]->reply(web::http::status_codes::OK, jsonResponse);
                Responses[i].reset();
            }
            else
                Pendingrefresh[i]									= ST_Refresh;
        }

	return 0;
}

::nwol::error_t					xover::CDealerTable::NextPlayer					(web::http::http_request message)			{
    size_t													player							= CurrentPlayer+1;

    for (; player < Players.size(); player++) {
        pplx::extensibility::scoped_critical_section_t lck(ResponseLock);

        if(	Players[player].Hand.Bet	>	0 
		 &&	Players[player].Hand.State	==	CBJHand::HR_Active 
		 ) {
			web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
			CBJPutResponse											putResponse						= {ST_YourTurn, jsonTable};
			web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
			Responses[player]->reply(web::http::status_codes::OK, jsonResponse);            
            Responses[player].reset();
            break;
        }
    }

    CurrentPlayer										= (int)player;

    if ( CurrentPlayer == ((int32_t)Players.size()) )
        CurrentPlayer										= 0;

    if ( CurrentPlayer == 0 )
        DealerDeal();

	return 0;
}

::nwol::error_t					xover::CDealerTable::PayUp							(size_t idx)								{
	CPlayer													& player						= Players[idx];
		 if ( player.Hand.Result == CBJHand::HR_PlayerWin		) { player.Balance += player.Hand.Bet*2;	player.Hand.Bet = 0.0;	}
	else if ( player.Hand.Result == CBJHand::HR_ComputerWin		) {											player.Hand.Bet = 0.0;	}
	else if ( player.Hand.Result == CBJHand::HR_PlayerBlackJack	) { player.Balance += player.Hand.Bet*2.5;	player.Hand.Bet = 0.0;	}
	else if ( player.Hand.Result == CBJHand::HR_Push			) { player.Balance += player.Hand.Bet;		player.Hand.Bet = 0.0;	}

	// Handle insurance
	if ( player.Hand.Insurance > 0 && Players[0].Hand.State == CBJHand::HR_PlayerBlackJack )
		player.Balance										+= player.Hand.Insurance*3;
	player.Hand.Insurance								= 0;

	pplx::extensibility::scoped_critical_section_t lck(ResponseLock);

	if (Responses[idx]) {
 		web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
		CBJPutResponse											putResponse						= {ST_PlaceBet, jsonTable};
		web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
		Responses[idx]->reply(web::http::status_codes::OK, jsonResponse);
		Responses[idx].reset();
	}
	else
		Pendingrefresh[idx] = ST_PlaceBet;

	return 0;
}

::nwol::error_t					xover::CDealerTable::Hit							(web::http::http_request message)			{
	SCard													card							= Shoe.front(); 
	Shoe.pop();
	Players[CurrentPlayer].Hand.AddCard(card);

	if( Players[CurrentPlayer].Hand.State == CBJHand::HR_BlackJack 
		|| Players[CurrentPlayer].Hand.State == CBJHand::HR_Busted 
		)
		Stay(message);
	else {
 		web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
		CBJPutResponse											putResponse						= {ST_YourTurn, jsonTable};
		web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
		message.reply(web::http::status_codes::OK, jsonResponse);
		pplx::extensibility::scoped_critical_section_t			lck								(ResponseLock);

		for (size_t i = 1; i < Players.size(); i++)
			if ( ((int32_t)i) != CurrentPlayer ) {
				if (Responses[i]) {
					putResponse											= {ST_Refresh, jsonTable};
  					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
					Responses[i]->reply(web::http::status_codes::OK, jsonResponse);
					Responses[i].reset();
				}
				else
					Pendingrefresh[i]									= ST_Refresh;
			}
	}

	return 0;
}

void							xover::CDealerTable::DoubleDown					(web::http::http_request message)			{
	if ( CurrentPlayer == 0 || Players[CurrentPlayer].Hand.State != CBJHand::HR_Active ) {
		message.reply(web::http::status_codes::Forbidden, U("Not your turn"));
		return; 
	}

	CPlayer													& current						= Players[CurrentPlayer];

	if ( current.Balance < current.Hand.Bet )	{ message.reply(web::http::status_codes::Forbidden, U("Not enough money"	)); return; }
	if ( current.Hand.Cards.size() > 2 )		{ message.reply(web::http::status_codes::Forbidden, U("Too many cards"		)); return; }

	// Double the bet

	current.Balance										-= current.Hand.Bet;
	current.Hand.Bet									*= 2;

	// Take one card and then stay

	SCard													card							= Shoe.front(); 
	Shoe.pop();
	Players[CurrentPlayer].Hand.AddCard(card);

	Stay(message);
}

::nwol::error_t					xover::CDealerTable::Wait							(web::http::http_request message)			{
	utility::string_t										name;
	auto													query							= web::http::uri::split_query(web::http::uri::decode(message.relative_uri().query()));
	auto													itr								= query.find(QUERY_NAME);
	if ( itr == query.end() ) {
		message.reply(web::http::status_codes::Forbidden, U("name and amount are required in query"));
		return 0;
	}
	else
		name												= itr->second;

	int														playerIdx						= FindPlayer(name);

	if ( playerIdx > 0 ) {
		pplx::extensibility::scoped_critical_section_t			lck								(ResponseLock);

		if ( Pendingrefresh[playerIdx] != ST_None ) {
 			web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
			CBJPutResponse											putResponse						= {Pendingrefresh[playerIdx], jsonTable};
			web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
			message.reply(web::http::status_codes::OK, jsonResponse);
			Pendingrefresh[playerIdx]							= ST_None;
		}
		else
			Responses[playerIdx]								= TMessageWrapper(new web::http::http_request(message));
	}

	return 0;
}

::nwol::error_t					xover::CDealerTable::Bet							(web::http::http_request message)			{
	int														amount;
	utility::string_t										name;
	TQueryMap												query							= web::http::uri::split_query(web::http::uri::decode(message.relative_uri().query()));
	TQueryMap::const_iterator								itrAmount						= query.find(AMOUNT)
		,													itrName							= query.find(QUERY_NAME)
		;
	if ( itrAmount == query.end() || itrName == query.end() ) {
		message.reply(web::http::status_codes::Forbidden, U("name and amount are required in query"));
		return 0;
	}
	utility::istringstream_t								ss								(itrAmount->second);
	ss													>> amount;
	name												= itrName->second;

	int														playerIdx						= FindPlayer(name);

	if ( playerIdx > 0 ) {
		Players[playerIdx].Balance							-= amount;
		Players[playerIdx].Hand.Bet							+= amount;
	}

	BetsMade											+= 1;
	Responses[playerIdx]								= TMessageWrapper(new web::http::http_request(message));

	if ( BetsMade == ((int32_t)Players.size()) - 1 )
		reterr_error_if(errored(Deal()), "%s() failed.", "Deal");

	// It is **possible** that all players have Blackjack, in which case the round is over. 
	// It's especially likely when there's only one player at the table.
	if ( CurrentPlayer == ((int32_t)Players.size()) ) {
		CurrentPlayer										= 0;
		DealerDeal();
	}

	return 0;
}

::nwol::error_t					xover::CDealerTable::Insure						(web::http::http_request message)			{
    int														amount;
    utility::string_t										name;

    TQueryMap												query							= web::http::uri::split_query(web::http::uri::decode(message.relative_uri().query()));
    auto													itrAmount						= query.find(AMOUNT), itrName = query.find(QUERY_NAME);
    if ( itrAmount == query.end() || itrName == query.end() ) {
        message.reply(web::http::status_codes::Forbidden, U("name and amount are required in query"));
        return 0;
    }
    utility::istringstream_t								ss(itrAmount->second);
    ss													>> amount;
    name												= itrName->second;
    int														playerIdx						= FindPlayer(name);

    if ( playerIdx > 0 ) {
        const CBJHand											& dealer						= Players[0].Hand;
        if ( Players[playerIdx].Hand.Insurance > 0.0 ) {
            message.reply(web::http::status_codes::Forbidden, U("Already insured")); 
            return 0; 
        }
        if ( dealer.Cards.size() < 1 || dealer.RevealBoth || dealer.Cards[0].Value != SCard::CV_Ace ) {
            message.reply(web::http::status_codes::Forbidden, U("Dealer is not showing an Ace")); 
            return 0; 
        }
        Players[playerIdx].Balance							-= amount;
        Players[playerIdx].Hand.Insurance					+= amount;
    }
 	web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
	CBJPutResponse											putResponse						= {ST_YourTurn, jsonTable};
	web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
    message.reply(web::http::status_codes::OK, jsonResponse);

	return 0;
}

::nwol::error_t					xover::CDealerTable::Stay							(web::http::http_request message)			{
    if ( 0 == CurrentPlayer ) 
		return 0;

    if ( Players[CurrentPlayer].Hand.State == CBJHand::HR_Active )
        Players[CurrentPlayer].Hand.State					= CBJHand::HR_Held;

 	web::json::value										jsonTable;						reterr_error_if(errored(AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
	CBJPutResponse											putResponse						= {ST_Refresh, jsonTable};
	web::json::value										jsonResponse;					reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
    message.reply(web::http::status_codes::OK, jsonResponse);

	NextPlayer(message);

	return 0;
}

void							xover::CDealerTable::DealerDeal						()											{
	CBJHand													& dealersHand					= Players[0].Hand;
	dealersHand.RevealBoth								= true;

	SNumericHandValues										handValue						= dealersHand.GetNumericValues();
	while ( handValue.High < 17 || (handValue.High > 21 && handValue.Low < 17) ) {
		SCard													card							= Shoe.front(); Shoe.pop();
		dealersHand.AddCard(card);

		if ( dealersHand.State == CBJHand::HR_BlackJack || Players[CurrentPlayer].Hand.State == CBJHand::HR_Busted )
			break;

		handValue											= dealersHand.GetNumericValues();
	}

	for (size_t i = 1; i < Players.size(); i++) {
		CPlayer													& player						= Players[i];

		if( player.Hand.Bet		== 0.0						) { player.Hand.Result		= CBJHand::HR_None;																					continue; }
		if( player.Hand.State	== CBJHand::HR_Busted		) { player.Hand.Result		= CBJHand::HR_ComputerWin;																			continue; }
		if( player.Hand.State	== CBJHand::HR_BlackJack	) { player.Hand.Result		= (dealersHand.State == CBJHand::HR_BlackJack) ? CBJHand::HR_Push : CBJHand::HR_PlayerBlackJack;	continue; }
		if( dealersHand.State	== CBJHand::HR_Busted		) { player.Hand.Result		= CBJHand::HR_PlayerWin;																			continue; }
		if( dealersHand.State	== CBJHand::HR_BlackJack	) { player.Hand.Result		= CBJHand::HR_ComputerWin;																			continue; }
		SNumericHandValues										value							= player.Hand.GetNumericValues();
		player.Hand.Result									= 
			(value.Best() <  handValue.Best()) ? CBJHand::HR_ComputerWin : 
			(value.Best() == handValue.Best()) ? CBJHand::HR_Push : CBJHand::HR_PlayerWin;
	}

	Betting											= true;

	for (size_t i = 1; i < Players.size(); i++)
		PayUp(i);

	BetsMade											= 0;
}



void							xover::CDealerTable::FillShoe						(size_t decks)								{
    // Value == suit*16+(facevalue-1)

    // Stack the decks.
    std::shared_ptr<int>									ss								(new int[decks * 52]);
    auto													shoe							= ss.get();

    for (size_t d = 0; d < decks ; d++)
        for (int suit = 0; suit < 4; suit++)
            for (int fv = 0; fv < 13; fv++)
                shoe[d*52+suit*13+fv]								= suit*16+(fv+1);

    // Randomize the shoe
#ifdef _WIN32
    LARGE_INTEGER											li;
    QueryPerformanceCounter(&li);
    std::mt19937											eng								(li.LowPart);
#else
    struct timeval											val;
    gettimeofday(&val, nullptr);
    std::mt19937											eng								(val.tv_usec);
#endif
    std::uniform_int_distribution<int>						dist							(0,(int)(decks*52-1));

#define ITER 4
    for (size_t r = 0; r < ITER; r++)
        for (size_t i = 0; i < decks*52; i++) {
            int														other							= dist(eng);
            std::swap(shoe[i],shoe[other]);
        }

    // Convert to the other format
    for (size_t i = 0; i < decks*52; i++) {
        SCard													card;
        card.Suit											= (SCard::ESuit		)(shoe[i] / 16);
        card.Value											= (SCard::EValue	)(shoe[i] % 16);
        Shoe.push(card);
    }

    StopAt												= std::uniform_int_distribution<int>(26,52)(eng);
}
