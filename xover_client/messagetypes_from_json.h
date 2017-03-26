#include "../xover_server/messagetypes.h"

namespace xover 
{
	static				::nwol::error_t						FromJSON							(const web::json::value		& value, CBJPutResponse& outputResponse)					{ 
		CBJPutResponse												result;
		const web::json::object										object								= value.as_object();
		const web::json::object::const_iterator						endElem								= object.end();
		web::json::object::const_iterator 
		objectIterator	= object.find(STATUS	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "STATUS"	) else result.Status	= (xover::EBJStatus)objectIterator->second.as_integer();
		objectIterator	= object.find(DATA		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "DATA"		) else result.Data		= objectIterator->second;
		outputResponse											=  result;
		return 0; 
	}
	static				::nwol::error_t						FromJSON							(const web::json::object	& object, SCard			& outputCard)						{
		SCard														result;
		const web::json::object::const_iterator						endElem								= object.end();
		web::json::object::const_iterator 
		objectIterator	= object.find(SUIT	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "SUIT"		) else result.Suit	= (SCard::ESuit )objectIterator->second.as_integer();
		objectIterator	= object.find(VALUE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "VALUE"	) else result.Value	= (SCard::EValue)objectIterator->second.as_integer();
		outputCard												= result;
		return 0;
	}
	static				::nwol::error_t						FromJSON							(const web::json::object &object, CBJHand& outputHand)									{
		CBJHand														result;
		web::json::value											cs									= object.at(CARDS);
		for (auto iter = cs.as_array().begin(); iter != cs.as_array().end(); ++iter) {
			if ( !iter->is_null() ) {
				SCard														card;
				FromJSON(iter->as_object(), card);
				result.Cards.push_back(card);
			}
		}
		const web::json::object::const_iterator						endElem								= object.end();
		web::json::object::const_iterator
		objectIterator	= object.find(STATE		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "STATE"		); result.State		= (CBJHand::EBJHandState)	objectIterator->second.as_integer	();	
		objectIterator	= object.find(RESULT	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "RESULT"		); result.Result	= (CBJHand::EBJHandResult)	objectIterator->second.as_integer	();
		objectIterator	= object.find(BET		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "BET"			); result.Bet		=							objectIterator->second.as_double	();	
		objectIterator	= object.find(INSURANCE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "INSURANCE"	); result.Insurance	=							objectIterator->second.as_double	();	
		outputHand												= result;
		return 0;
	}
	static				::nwol::error_t						FromJSON							(const web::json::object& object, CPlayer& outputPlayer)								{
		CPlayer														result								= {U("")};
		const web::json::object::const_iterator						endElem								= object.end();
		web::json::object::const_iterator 
		objectIterator	= object.find(NAME		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "NAME"		) else result.Name		= objectIterator->second.as_string();
		objectIterator	= object.find(BALANCE	);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "BALANCE"	) else result.Balance	= objectIterator->second.as_double();
		objectIterator	= object.find(HAND		);	reterr_error_if(objectIterator == endElem, "key not found: %s.", "HAND"		) 
		::nwol::error_t												errMy								=  FromJSON(objectIterator->second.as_object(), result.Hand);
		reterr_error_if_errored(errMy, "%s", "Failed to get hand from JSON.");
	
		outputPlayer											= result;
		return 0;
	}
	static				::nwol::error_t						FromJSON							(const web::json::object &object, CBJTable& outputTable)								{
		CBJTable													result;
		web::json::object::const_iterator 
		objectIterator		= object.find(ID		);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "ID"		) else result.Id		= (int32_t)			objectIterator->second.as_double();
		objectIterator		= object.find(CAPACITY	);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "CAPACITY"	) else result.Capacity	= (uint32_t)	objectIterator->second.as_double();
		objectIterator		= object.find(PLAYERS	);	reterr_error_if(objectIterator == object.end(), "key not found: %s", "PLAYERS"	);
		web::json::value											players								= objectIterator->second;
		int															i									= 0;
		for (auto iter = players.as_array().begin(); iter != players.as_array().end(); ++iter, i++) {
			CPlayer														player;
			::nwol::error_t												errMy								= FromJSON(iter->as_object(), player);
			reterr_error_if_errored(errMy, "%s", "Error getting player from JSON.");
			result.Players.push_back(player);
		}
	
		outputTable												= result;
		return 0;
	}
} // namespace