#include "xover_client.h"

#include <SDKDDKVer.h>	// Including SDKDDKVer.h defines the highest available Windows platform.

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "XOver Client", 0, 1);

#include "messagetypes_from_json.h"
//namespace xover 
//{
//	static				::nwol::error_t						FromJSON							(const web::json::value		& value, CBJPutResponse& outputResponse)					{ 
//		CBJPutResponse												result;
//		const web::json::object										object								= value.as_object();
//		const web::json::object::const_iterator						endElem								= object.end();
//		web::json::object::const_iterator 
//		objectIterator	= object.find(STATUS	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "STATUS"	) else result.Status	= (xover::EBJStatus)objectIterator->second.as_integer();
//		objectIterator	= object.find(DATA		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "DATA"		) else result.Data		= objectIterator->second;
//		outputResponse											=  result;
//		return 0; 
//	}
//	static				::nwol::error_t						FromJSON							(const web::json::object	& object, SCard			& outputCard)						{
//		SCard														result;
//		const web::json::object::const_iterator						endElem								= object.end();
//		web::json::object::const_iterator 
//		objectIterator	= object.find(SUIT	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "SUIT"		) else result.Suit	= (SCard::ESuit )objectIterator->second.as_integer();
//		objectIterator	= object.find(VALUE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "VALUE"	) else result.Value	= (SCard::EValue)objectIterator->second.as_integer();
//		outputCard												= result;
//		return 0;
//	}
//	static				::nwol::error_t						FromJSON							(const web::json::object &object, CBJHand& outputHand)									{
//		CBJHand														result;
//		web::json::value											cs									= object.at(CARDS);
//		for (auto iter = cs.as_array().begin(); iter != cs.as_array().end(); ++iter) {
//			if ( !iter->is_null() ) {
//				SCard														card;
//				FromJSON(iter->as_object(), card);
//				result.Cards.push_back(card);
//			}
//		}
//		const web::json::object::const_iterator						endElem								= object.end();
//		web::json::object::const_iterator
//		objectIterator	= object.find(STATE		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "STATE"		); result.State		= (CBJHand::EBJHandState)	objectIterator->second.as_integer	();	
//		objectIterator	= object.find(RESULT	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "RESULT"		); result.Result	= (CBJHand::EBJHandResult)	objectIterator->second.as_integer	();
//		objectIterator	= object.find(BET		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "BET"			); result.Bet		=							objectIterator->second.as_double	();	
//		objectIterator	= object.find(INSURANCE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "INSURANCE"	); result.Insurance	=							objectIterator->second.as_double	();	
//		outputHand												= result;
//		return 0;
//	}
//	static				::nwol::error_t						FromJSON							(const web::json::object& object, CPlayer& outputPlayer)								{
//		CPlayer														result								= {U("")};
//		const web::json::object::const_iterator						endElem								= object.end();
//		web::json::object::const_iterator 
//		objectIterator	= object.find(NAME		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "NAME"		) else result.Name		= objectIterator->second.as_string();
//		objectIterator	= object.find(BALANCE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "BALANCE"	) else result.Balance	= objectIterator->second.as_double();
//		objectIterator	= object.find(HAND		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "HAND"		) 
//		::nwol::error_t												errMy								=  FromJSON(objectIterator->second.as_object(), result.Hand);
//		reterr_error_if_errored(errMy, "%s", "Failed to get hand from JSON.");
//	
//		outputPlayer											= result;
//		return 0;
//	}
//	static				::nwol::error_t						FromJSON							(const web::json::object &object, CBJTable& outputTable)								{
//		CBJTable													result;
//		web::json::object::const_iterator 
//		objectIterator		= object.find(ID		);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "ID"		) else result.Id		= (int32_t)			objectIterator->second.as_double();
//		objectIterator		= object.find(CAPACITY	);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "CAPACITY"	) else result.Capacity	= (uint32_t)	objectIterator->second.as_double();
//		objectIterator		= object.find(PLAYERS	);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "PLAYERS"	);
//		web::json::value											players								= objectIterator->second;
//		int															i									= 0;
//		for (auto iter = players.as_array().begin(); iter != players.as_array().end(); ++iter, i++) {
//			CPlayer														player;
//			::nwol::error_t												errMy								= FromJSON(iter->as_object(), player);
//			reterr_error_if_errored(errMy, "%s", "Error getting player from JSON.");
//			result.Players.push_back(player);
//		}
//	
//		outputTable												= result;
//		return 0;
//	}
//} // namespace
//#ifdef _WIN32
//# define iequals(x, y) (_stricmp((x), (y))==0)
//#else
//# define iequals(x, y) boost::iequals((x), (y))
//#endif

//static inline	web::http::http_response		checkResponse						(const web::http::http_response &response)													{
//    ucout											<< response.to_string() << std::endl;	
//    return response;
//}
//
//web::http::http_response						checkResponse						(const web::http::http_response &response, bool &refresh)									{
//    ucout											<< response.to_string() << std::endl;
//    //::xover::CBJPutResponse								answer;
//	//::xover::FromJSON(response.extract_json().get(), answer);
//    //refresh											= answer.Status == ::xover::ST_Refresh;
//    return response;
//}
//
//void											printResult							(::xover::CBJHand::EBJHandResult result)													{
//    switch (result) {
//    case ::xover::CBJHand::HR_PlayerBlackJack	: ucout	<< "Black Jack"		; break;
//    case ::xover::CBJHand::HR_PlayerWin			: ucout	<< "Player wins"	; break;
//    case ::xover::CBJHand::HR_ComputerWin		: ucout	<< "Computer Wins"	; break;
//    case ::xover::CBJHand::HR_Push				: ucout	<< "Push"			; break;
//    }
//}
//
//void											printCard							(const ::xover::SCard &card)																{
//    switch (card.Value) {
//    case ::xover::SCard::CV_King	: ucout				<< "K"				; break;
//    case ::xover::SCard::CV_Queen	: ucout				<< "Q"				; break;
//    case ::xover::SCard::CV_Jack	: ucout				<< "J"				; break;
//    case ::xover::SCard::CV_Ace		: ucout				<< "A"				; break;
//    default							: ucout				<< (int)card.Value	; break;
//    }
//    switch (card.Suit) {
//    case ::xover::SCard::CS_Club	: ucout				<< "C"				; break;
//    case ::xover::SCard::CS_Spade	: ucout				<< "S"				; break;
//    case ::xover::SCard::CS_Heart	: ucout				<< "H"				; break;
//    case ::xover::SCard::CS_Diamond	: ucout				<< "D"				; break;
//    }
//}
//
//void											printHand						(bool suppress_bet, const ::xover::CBJHand &hand)												{
//    if (!suppress_bet) {
//        if ( hand.Insurance > 0 )
//            ucout											<< "Bet: " << hand.Bet << "Insurance: " << hand.Insurance << " Hand: ";
//        else
//            ucout											<< "Bet: " << hand.Bet << " Hand: ";
//    }
//    for (auto iter = hand.Cards.begin(); iter != hand.Cards.end(); iter++) {
//        printCard(*iter); 
//		ucout											<< " ";
//	}
//    printResult(hand.Result);
//}
////-----------------------------
//void											printTable						(const web::http::http_response &response, bool &refresh)										{
//	refresh											= false;
//
//	if ( response.status_code() == web::http::status_codes::OK ) {
//        if ( response.headers().content_type() == U("application/json") ) {
//            ::xover::CBJPutResponse								answer;
//			FromJSON(response.extract_json().get(), answer);
//            web::json::value									players							= answer.Data[PLAYERS];
//
//            refresh											= answer.Status == ::xover::ST_Refresh;
//
//            for (auto iter = players.as_array().begin(); iter != players.as_array().end(); ++iter) {
//                auto												& player						= *iter;
//                web::json::value									name							= player[NAME];
//                web::json::value									bet								= player[BALANCE];
//                bool												suppressMoney					= iter == players.as_array().begin();
//                if ( suppressMoney )
//                    ucout											<< "'" << name.as_string() << "'" ;
//                else
//                    ucout											<< "'" << name.as_string() << "' Balance = $" << bet.as_double() << " ";
//
//				::xover::CBJHand									hand;
//				::xover::FromJSON(player[HAND].as_object(), hand);
//                printHand(suppressMoney, hand);
//                ucout											<< std::endl;
//            }
//
//            switch ( answer.Status ) {
//            case ::xover::ST_PlaceBet: ucout					<< "Place your bet!\n"	; break;
//            case ::xover::ST_YourTurn: ucout					<< "Your turn!\n"		; break;
//            }
//        }
//    }
//}
////-----------------------------
//::nwol::error_t									tickTable						(::SApplication& instanceApp)																	{
//	web::http::client::http_client						& bjDealer						= *instanceApp.BJDealer.get();
//	web::json::value									& availableTables				= instanceApp.AvailableTables;
//	bool												& wasRefresh					= instanceApp.WasRefresh;
//	utility::string_t									& userName						= instanceApp.UserName;
//	utility::string_t									& tableName						= instanceApp.Table;
//
//    while ( wasRefresh ) {
//        wasRefresh									= false;
//        utility::ostringstream_t						buf;
//        buf											<< tableName << U("?request=refresh&name=") << userName;
//        printTable(checkResponse(bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
//    }
//
//	std::string											method;
//    ucout											<< "Enter method:";
//    std::cin										>> method;
//    if ( iequals(method.c_str(), "quit") ) {
//        if ( !userName.empty() && !tableName.empty() ) {
//            utility::ostringstream_t							buf;
//            buf												<< tableName << U("?name=") << userName;
//            checkResponse(bjDealer.request(web::http::methods::DEL, buf.str()).get());
//        }
//        return 1;
//    }
//    if ( iequals(method.c_str(), "name") ) {
//        ucout											<< "Enter user name:";
//        ucin											>> userName;
//    }
//    else if ( iequals(method.c_str(), "join") ) {
//        ucout											<< "Enter table name:";
//        ucin											>> tableName;
//        if ( userName.empty() ) { 
//			ucout											<< "Must have a name first!\n"; 
//			return 0; 
//		}
//        utility::ostringstream_t							buf;
//        buf												<< tableName << U("?name=") << userName;
//        checkResponse(bjDealer.request(web::http::methods::POST, buf.str()).get(), wasRefresh);
//    }
//    else if(iequals(method.c_str(), "hit"	)
//        ||	iequals(method.c_str(), "stay"	)
//        ||	iequals(method.c_str(), "double") 
//		)
//    {
//        utility::ostringstream_t							buf;
//        buf												<< tableName << U("?request=") << utility::conversions::to_string_t(method) << U("&name=") << userName;
//        printTable(checkResponse(bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
//    }
//    else if(iequals(method.c_str(), "bet"	)	 
//		||	iequals(method.c_str(), "insure") 
//		)
//    {
//        utility::string_t									bet;
//        ucout											<< "Enter bet:";
//        ucin											>> bet;
//
//        if ( userName.empty() ) { 
//			ucout											<< "Must have a name first!\n"; 
//			return 0; 
//		}
//
//        utility::ostringstream_t							buf;
//        buf												<< tableName << U("?request=") << utility::conversions::to_string_t(method) << U("&name=") << userName << U("&amount=") << bet;
//        printTable(checkResponse(bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
//    }
//    else if ( iequals(method.c_str(), "newtbl") )
//        checkResponse(bjDealer.request(web::http::methods::POST).get(), wasRefresh);
//    else if ( iequals(method.c_str(), "leave") ) {
//        ucout											<< "Enter table:";
//        ucin											>> tableName;
//
//        if ( userName.empty() ) { 
//			ucout											<< "Must have a name first!\n"; 
//			return 0; 
//		}
//
//        utility::ostringstream_t							buf;
//        buf												<< tableName << U("?name=") << userName;
//        checkResponse(bjDealer.request(web::http::methods::DEL, buf.str()).get(), wasRefresh);
//    }
//    else if ( iequals(method.c_str(), "list") ) {
//        wasRefresh										= false;
//        web::http::http_response							response						= checkResponse(bjDealer.request(web::http::methods::GET).get());
//
//        if ( response.status_code() == web::http::status_codes::OK ) {
//            availableTables										= response.extract_json().get();
//            for (auto iter = availableTables.as_array().begin(); iter != availableTables.as_array().end(); ++iter) {
//                ::xover::CBJTable									bj_table;
//				::xover::FromJSON(iter->as_object(), bj_table);
//                web::json::value									id								= web::json::value::number(bj_table.Id);
//                ucout											<< "table " << bj_table.Id << ": {capacity: " << (long unsigned int)bj_table.Capacity << " no. players: " << (long unsigned int)bj_table.Players.size() << " }\n";
//            }
//            ucout											<< std::endl;
//        }
//    }
//    else {
//        ucout											<< utility::conversions::to_string_t(method) << ": not understood\n";
//    }
//
//    return 0;
//}
//-----------------------------
static inline	void							client_shutdown							()																						{}
				int32_t							client_init								(::SApplication& instanceApp, const utility::string_t& address)							{
	web::http::uri										uri										= web::http::uri(address);
	instanceApp.GameClient							=  std::make_shared<web::http::client::http_client>( web::http::uri_builder(uri).append_path(U("/blackjack/dealer")).to_uri() );

	instanceApp.AvailableTables						= web::json::value::array();
    instanceApp.WasRefresh							= false;

	return 0;
}

GDEFINE_ENUM_TYPE(CLIENT_ACTION, uint8_t);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x00, List		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x01, Join		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x02, Turn		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x03, Quit		);
										
//-----------------------------			
int32_t											setupREST					(::SApplication& instanceApp)																	{ 
	utility::string_t									port						= U("34568");
    if(3 <= instanceApp.RuntimeValues->CommandLineArgCount) {
#if defined(__WINDOWS__)
		wchar_t												temp[8]						= {};
		size_t												numCharsConverted			= 0;
		mbstowcs_s(&numCharsConverted, temp, instanceApp.RuntimeValues->CommandLineArgList[2], 5);
		port											= temp;
#else
		port											= instanceApp.RuntimeValues->CommandLineArgList[2];
#endif
	}
    utility::string_t									address							= U("http://localhost:");
    address.append(port);

	return client_init(instanceApp, address);
}

int32_t										setupGUI						(::SApplication& instanceApp)																	{ 
	::nwol::SGUI									& guiSystem						= instanceApp.GUI;

	// Create exit button
	::nwol::glabel									newControlLabel					= "Exit";
	::nwol::SGUIControl								newControl;
	newControl.AreaASCII						= {{1, 1}, {(int32_t)newControlLabel.size(), 1}}	;
	newControl.Text								= newControlLabel									;
	::nwol::error_t									errMy							= ::nwol::createControl(guiSystem, newControl);
	reterr_msg_if_error(errMy, "%s: \"%s\".", "Failed to create control", newControlLabel.begin());

	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_VCENTER)	;
	for(int32_t y=0; y<3; ++y) 
		for(int32_t x=0; x<3; ++x) {
			newControl.AreaASCII						= {{-3+x*3, -3+y}, {3, 1}}								;
			newControl.Text								= ::nwol::glabel::statics().space						;
			::nwol::SControlTextColorASCII					& colorsConsole				= newControl.TextColorsASCII;
			colorsConsole.Color.Background				= ((y*3+x) % 2) ? COLOR_DARKGREY : COLOR_WHITE			;
			colorsConsole.Color.Foreground				= COLOR_YELLOW											;
			colorsConsole.ColorPressed					= {COLOR_DARKGREY, COLOR_YELLOW}						;
			//newControl.AlignText						= ::nwol::SCREEN_CENTER									; this is not working yet
			::nwol::createControl(guiSystem, newControl);
		}

	newControlLabel								= "Restart"																;
	newControl									= ::nwol::SGUIControl()													;
	newControl.AlignArea						= (::nwol::ALIGN_SCREEN)(::nwol::SCREEN_CENTER | ::nwol::SCREEN_BOTTOM)	;
	newControl.AreaASCII						= {{0, 1}, {(int32_t)newControlLabel.size(), 1}}						;
	newControl.Text								= newControlLabel														;
	errMy										= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "Failed to create control!");

	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int32_t										cleanup							(::SApplication&)																				{
	::nwol::shutdownASCIIScreen();

	::client_shutdown();
	return 0; 
}

int32_t										setup							(::SApplication& instanceApp)																	{ 
	srand((unsigned int)time(0));

	::nwol::SGUI									& guiSystem						= instanceApp.GUI;
	int32_t											screenWidth						= guiSystem.TargetSizeASCII.x;
	int32_t											screenHeight					= guiSystem.TargetSizeASCII.y;
	::nwol::initASCIIScreen(screenWidth, screenHeight);
	::nwol::setASCIIScreenTitle(appTitle());

	::nwol::error_t									
	errMy										= ::setupREST		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup REST."		);
	errMy										= ::setupGUI		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup GUI."			);

	return 0; 
}

#if defined(__WINDOWS__)
#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif
#else 
#error "Unsupported platform. Define key code for this platform in order to continue."
#endif

int32_t										update							(::SApplication& instanceApp, bool exitRequested)												{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	// Update subsystems
	::nwol::SInput									& inputSystem					= instanceApp.Input;	::nwol::pollInput(inputSystem);
	::nwol::SGUI									& guiSystem						= instanceApp.GUI;		::nwol::updateGUI(guiSystem, inputSystem);
	::nwol::SGUIControlTable						& guiControls					= guiSystem.Controls;	

	// Update game
	::ttt::TicTacToe								& game							= instanceApp.Game;
	::ttt::TicTacToeBoard32							& gameBoard						= game.Board;
	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags					= guiControls.ControlFlags;

	const ::ttt::PLAYER_CONTROL						currentPlayer					= gameBoard.GetPlayerControlCurrent();
	::ttt::CellPick									cellPicked;
	static uint8_t									tickDelay						= 0;
	bool											handledControlEvent				= false;
	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl) {
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			debug_printf("Execute %u.", iControl);
			switch(iControl) {
			case 0:		return ::nwol::APPLICATION_STATE_EXIT; // Exit button clicked.
			case 10:	// Restart button clicked.
				game.Restart();

				for(int32_t y=0; y<3; ++y) {		// Clear board controls.
					for(int32_t x=0; x<3; ++x){
						int												 controlIndex						= (y*3+x)+1;
						guiControls.Text								[controlIndex]						= ::nwol::glabel::statics().space;
						guiControls.TextColorsASCII						[controlIndex].Color.Foreground		= COLOR_YELLOW;
						guiControls.TextColorsASCII						[controlIndex].ColorPressed			= {COLOR_DARKGREY, COLOR_YELLOW};
					}
				}
				debug_printf("%s", "Game restarted.");
				handledControlEvent							= true;
				break;

			default:	// Board cell clicked 
				if(::ttt::PLAYER_CONTROL_HUMAN == currentPlayer && gameBoard.MovesLeft && !gameBoard.Winner) {
					uint8_t											cellIndex						= (uint8_t)(iControl-1);
					cellPicked									= {::ttt::CELL_VALUE_EMPTY, cellIndex, (uint8_t)gameBoard.PlayerIndex};
					if(::ttt::CELL_VALUE_EMPTY == gameBoard.GetCellValue(cellIndex)) {
						cellPicked									= instanceApp.Game.Tick(cellIndex);
						tickDelay									= 2;
					}
					int32_t											playerNumber					= cellPicked.IndexPlayer+1;
					const char										* textAction					= cellPicked.Value ? "Player %u played {%u, %u}" : "Player %u cannot move to {%u, %u}";
					always_printf(textAction, playerNumber, cellPicked.Row, cellPicked.Column);
				}
				handledControlEvent							= true;
				break;
			}
		}
		if(handledControlEvent)
			break;
	}

	// If the human didn't exit the game and it's not its turn, then let the computer play
	if(currentPlayer != ::ttt::PLAYER_CONTROL_HUMAN && gameBoard.MovesLeft && !gameBoard.Winner && !tickDelay && !cellPicked.Value)
		cellPicked									= game.Tick(0);

	if(cellPicked.Value) {	// If a cell was picked and its value changed, we need to change the colors of the cell control
		uint32_t										 controlIndex							= cellPicked.IndexCell+1;
		guiControls.TextColorsASCII						[controlIndex].Color		.Foreground	= 
		guiControls.TextColorsASCII						[controlIndex].ColorPressed	.Background	= cellPicked.IndexPlayer ? COLOR_RED : COLOR_BLUE;
		static const ::nwol::glabel						symbols[]								= {" X ", " O "};
		guiControls.Text								[controlIndex]							= symbols[cellPicked.IndexPlayer];
	}

	tickDelay									+= 2;	// Always decrease AI delay

	sizeof(::SApplication);
	sizeof(ttt::TicTacToe);
	sizeof(ttt::CellCoord);
	sizeof(ttt::CellPick);
	return 0; 
}

template<uint32_t _screenWidth, uint32_t _screenHeight>
void										bltASCIIScreen					(const ttt::ScreenASCII<_screenWidth, _screenHeight>& source, ::nwol::SASCIITarget& target)		{
	for(uint32_t y = 0, yMax = ::nwol::min(_screenHeight, target.Height()); y < yMax; ++y)
		for(uint32_t x = 0, xMax = ::nwol::min(_screenWidth, target.Width()); x < xMax; ++x)
			target.Text[y][x]							= source.Cells[y][x];
}

int32_t										render							(::SApplication& instanceApp)																	{ 
	static constexpr const uint32_t					screenWidth						= ttt::TicTacToe::SCREEN_WIDTH+1;
	static constexpr const uint32_t					screenHeight					= ttt::TicTacToe::SCREEN_HEIGHT;
	ttt::ScreenASCII<screenWidth, screenHeight>		targetScreenTTT					= {};

	const ::ttt::TicTacToe							& game							= instanceApp.Game;
	const ::ttt::TicTacToeBoard32					& gameBoard						= game.Board;
	if(gameBoard.Winner || !gameBoard.MovesLeft)
		game.DrawResults(gameBoard.GetWinner(), screenWidth>>1, (screenHeight>>1)+2, targetScreenTTT.Cells);

	game.DrawBoard(1, 1, targetScreenTTT.Cells);
	game.DrawBoard(::ttt::CELL_VALUE_X, 1, 5, targetScreenTTT.Cells);
	game.DrawBoard(::ttt::CELL_VALUE_O, 1, 9, targetScreenTTT.Cells);

	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							actualScreen					= {};
	::nwol::error_t									errMy							= ::nwol::getASCIIBackBuffer(actualScreen);	
	reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	bltASCIIScreen(targetScreenTTT, actualScreen);

	::nwol::renderGUIASCII(actualScreen, instanceApp.GUI);

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}
