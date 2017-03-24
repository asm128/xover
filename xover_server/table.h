// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
#include "cpprest/http_listener.h"

#ifndef __TABLE_H__92873492834__
#define __TABLE_H__92873492834__

namespace xover
{
	typedef	std::map<utility::string_t, utility::string_t>		TQueryMap;

	class CDealerTable : public CBJTable {
				int32_t												StopAt						= 0;
				std::queue<SCard>									Shoe						;
				bool												Betting						= true;

				pplx::extensibility::critical_section_t				ResponseLock				;

		typedef	std::shared_ptr<web::http::http_request>			TMessageWrapper				;

				std::vector<EBJStatus>								Pendingrefresh				= {ST_None,};
				std::vector<TMessageWrapper>						Responses					= {TMessageWrapper(),};

				int32_t												CurrentPlayer				= 0;
				int													BetsMade					= 0;

				void												FillShoe					(size_t decks);
				void												DealerDeal					();
				::nwol::error_t										PayUp						(size_t playerId);
				::nwol::error_t										NextPlayer					(web::http::http_request);
				int													FindPlayer					(const utility::string_t &name);
	public:
																	CDealerTable				()																	{ FillShoe(6);		}
																	CDealerTable				(int id, uint32_t capacity, int decks)	: CBJTable(id, capacity)	{ FillShoe(decks);	}

				::nwol::error_t										Deal						();

				::nwol::error_t										Hit							(web::http::http_request);
				::nwol::error_t										Stay						(web::http::http_request); // True if it's now your turn again, such as when you're the only player...
				void												DoubleDown					(web::http::http_request);
				::nwol::error_t										Bet							(web::http::http_request);
				::nwol::error_t										Insure						(web::http::http_request);
				::nwol::error_t										Wait						(web::http::http_request);

				bool												AddPlayer					(const CPlayer &player);
				bool												RemovePlayer				(const utility::string_t &name);
	};
}	// namespace 
#endif // __DEALER_H__92873492834__

