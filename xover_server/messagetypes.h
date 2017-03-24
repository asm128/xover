#include "cpprest/json.h"
#include "cpprest/details/basic_types.h"
#include "log.h"
#include "nwol_error.h"
#include "enum.h"

#include <map>

#ifndef __MESSAGETYPES_H__9283492874__
#define __MESSAGETYPES_H__9283492874__

namespace xover
{
	//enum EBJPossibleMoves	{ PM_None = 0x0, PM_Hit = 0x1, PM_DoubleDown = 0x2, PM_Split = 0x4, PM_All = 0x7 };
	enum EBJStatus			{ ST_PlaceBet, ST_Refresh, ST_YourTurn, ST_None };

	GDEFINE_ENUM_TYPE(XOVER_ACTION, uint8_t);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, state		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, bet			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, double		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, insure		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, hit			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, stay			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, refresh		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, insurance	);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, result		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, suit			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, value		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, cards		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, request		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, amount		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, name			);

	GDEFINE_ENUM_TYPE(XOVER_QUERY, uint8_t);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Balance		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Hand			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Capacity		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Id			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Players		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Status		);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Data			);
	GDEFINE_ENUM_VALUE(XOVER_ACTION, 0x00, Name			);

	GDEFINE_ENUM_TYPE(XOVER_PLAYER, uint8_t);
	GDEFINE_ENUM_VALUE(XOVER_PLAYER, 0x00, DEALER		);

#define STATE			U("state"		)
#define BET				U("bet"			)
#define DOUBLEDOWN		U("double"		)
#define INSURE			U("insure"		)
#define HIT				U("hit"			)
#define STAY			U("stay"		)
#define REFRESH			U("refresh"		)
#define INSURANCE		U("insurance"	)
#define RESULT			U("result"		)
#define NAME			U("Name"		)
#define BALANCE			U("Balance"		)
#define HAND			U("Hand"		)
#define SUIT			U("suit"		)
#define VALUE			U("value"		)
#define CARDS			U("cards"		)
#define CAPACITY		U("Capacity"	)
#define ID				U("Id"			)
#define PLAYERS			U("Players"		)
#define DEALER			U("DEALER"		)
#define DATA			U("Data"		)
#define STATUS			U("Status"		)
#define REQUEST			U("request"		)
#define AMOUNT			U("amount"		)
#define QUERY_NAME		U("name"		)

	struct SCard {
		enum				ESuit								: uint8_t{ CS_Heart, CS_Diamond, CS_Club, CS_Spade }																						;
		enum				EValue								: uint8_t{ CV_None, CV_Ace, CV_Two, CV_Three, CV_Four, CV_Five, CV_Six, CV_Seven, CV_Eight, CV_Nine, CV_Ten, CV_Jack, CV_Queen, CV_King }	;

							ESuit								Suit;
							EValue								Value;

							web::json::value					AsJSON								()																	const				{
			web::json::value											result								= web::json::value::object();
			result[SUIT	]								= web::json::value::number(Suit);
			result[VALUE]								= web::json::value::number(Value);
			return result;
		}
	};

	struct SNumericHandValues {
							int									Low									= 0;
							int									High								= 0;

		inline constexpr	int									Best								()																	const	noexcept	{ return (High < 22) ? High : Low; }
	};

	struct CBJHand {
		enum				EBJHandState						: uint8_t{ HR_Empty, HR_BlackJack, HR_Active, HR_Held, HR_Busted				};
		enum				EBJHandResult						: uint8_t{ HR_None, HR_PlayerBlackJack, HR_PlayerWin, HR_ComputerWin, HR_Push	};

							std::vector<SCard>					Cards								= {};
							double								Bet									= 0;
							double								Insurance							= 0;
							bool								RevealBoth							= true;
							EBJHandState						State								= HR_Empty;
							EBJHandResult						Result								= HR_None;

							void								Clear								()																						{ Cards.clear(); State = HR_Empty; Result = HR_None; Insurance = 0.0; }
							void								AddCard								(SCard card)																			{ 
			Cards.push_back(card); 
			SNumericHandValues											value								= GetNumericValues();
				 if ( Cards.size() == 2 && value.High == 21 )	State	= HR_BlackJack;
			else if ( value.Low > 21 )							State	= HR_Busted;
			else												State	= HR_Active;
		}
							SNumericHandValues					GetNumericValues					()																						{
			SNumericHandValues											res;
			res.Low													= 0;
			bool														hasAces								= false;

			for (const SCard& card : Cards) {
				if ( card.Value == SCard::CV_Ace ) 
					hasAces													= true;
				res.Low													+= std::min((int)card.Value, 10);
			}
			res.High												= hasAces ? res.Low + 10 : res.Low;
			return res;
		}
							web::json::value					AsJSON								()																	const				{
			web::json::value											result								= web::json::value::object();
			result[STATE		]									= web::json::value::number(State		);
			result[RESULT		]									= web::json::value::number(Result		);
			result[BET			]									= web::json::value::number(Bet			);
			result[INSURANCE	]									= web::json::value::number(Insurance	);

			web::json::value											jCards									= web::json::value::array(Cards.size());
			if ( RevealBoth ) {
				int															idx										= 0;
				for (auto iter = Cards.begin(); iter != Cards.end(); ++iter)
					jCards[idx++]											= iter->AsJSON();
			}
			else {
				if(Cards.size())
					jCards[0]												= Cards.begin()->AsJSON();
			}
			result[CARDS		]									= jCards;
			return result;
		}

	};

	struct CPlayer {
							utility::string_t					Name;
							CBJHand								Hand;
							double								Balance;

		inline													CPlayer								()																						{}
		inline													CPlayer								(const utility::string_t &name)															: Name(name), Balance(1000.0) {}


							::nwol::error_t						AsJSON								(web::json::value& outputValue)										const				{
			web::json::value											result								= web::json::value::object();
			result[NAME		]							= web::json::value::string(Name);
			result[BALANCE	]							= web::json::value::number(Balance);
			result[HAND		]							= Hand.AsJSON();

			outputValue												= result;
			return 0;
		}
	};

	struct CBJTable {
							int32_t								Id			= -1;
							uint32_t							Capacity	= 0;
							std::vector<CPlayer>				Players		= {};

		inline constexpr										CBJTable							()																						= default;
		inline													CBJTable							(int32_t id, uint32_t capacity)															: Id(id), Capacity(capacity), Players{{DEALER}}  {}

							::nwol::error_t						AsJSON								(web::json::value& outputValue)										const				{
			web::json::value											result								= web::json::value::object();
			result[ID		]										= web::json::value::number((double)Id);
			result[CAPACITY	]										= web::json::value::number((double)Capacity);

			web::json::value											jPlayers							= web::json::value::array(Players.size());
			size_t														idx									= 0;
			for (auto iter = Players.begin(); iter != Players.end(); ++iter) {
				::nwol::error_t												errMy								= iter->AsJSON(jPlayers[idx++]);
				reterr_error_if_errored(errMy, "%s", "Failed to get player as JSON.");
			}
			result[PLAYERS]											= jPlayers;
			outputValue												= result;
			return 0;
		}
	};

	struct CBJPutResponse {
							EBJStatus							Status	;							
							web::json::value					Data	;

							::nwol::error_t						AsJSON								(web::json::value& outputValue)										const				{
			web::json::value											result								= web::json::value::object();
			result[STATUS	]										= web::json::value::number((double)Status);
			result[DATA		]										= Data;
			outputValue												= result;
			return 0;
		}
	};

	static inline		web::json::value					TablesAsJSON						(const std::map<utility::string_t, std::shared_ptr<CBJTable>> & tables)					{
		web::json::value											result								= web::json::value::array();
		size_t														idx									= 0;
		for (auto tbl = tables.begin(); tbl != tables.end(); tbl++) {
			const ::nwol::error_t										errMy 								= tbl->second->AsJSON(result[idx++]);
			reterr_error_if_errored(errMy, "%s", "Failed to get table from JSON.")
		}

		return result;
	}
} // namespace 

#endif // __MESSAGETYPES_H__9283492874__