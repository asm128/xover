// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
#include "messagetypes.h"
#include "Table.h"

#ifndef __DEALER_H__92873492834__
#define __DEALER_H__92873492834__

namespace xover
{
	// Contains the main logic of the black jack dealer.
	class CDealer {
		static std::map<utility::string_t, std::shared_ptr<CBJTable>>	s_tables						;
		static int														nextId							;

				web::http::experimental::listener::http_listener		Listener						;   

				::nwol::error_t											HandleGet						(web::http::http_request message);
				::nwol::error_t											HandlePut						(web::http::http_request message);
				::nwol::error_t											HandlePost						(web::http::http_request message);
				::nwol::error_t											HandleDelete					(web::http::http_request message);
	public:
																		CDealer							()									{}
																		CDealer							(utility::string_t url)				: Listener(url)				{
			Listener.support(web::http::methods::GET	, std::bind(&CDealer::HandleGet		, this, std::placeholders::_1));
			Listener.support(web::http::methods::PUT	, std::bind(&CDealer::HandlePut		, this, std::placeholders::_1));
			Listener.support(web::http::methods::POST	, std::bind(&CDealer::HandlePost	, this, std::placeholders::_1));
			Listener.support(web::http::methods::DEL	, std::bind(&CDealer::HandleDelete	, this, std::placeholders::_1));
    
			utility::ostringstream_t											nextIdString;
			nextIdString													<< nextId;

			std::shared_ptr<CDealerTable>										tbl								= std::make_shared<CDealerTable>(nextId, 8, 6);
			nextId															+= 1;
			s_tables[nextIdString.str()]									= tbl;
		}
		inline	pplx::task<void>										Open							()									{ return Listener.open	(); }
		inline	pplx::task<void>										Close							()									{ return Listener.close	(); }
	};
}
#endif // __DEALER_H__92873492834__