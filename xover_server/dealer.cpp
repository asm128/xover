// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
#include "Dealer.h"

typedef std::map<utility::string_t, std::shared_ptr<xover::CBJTable>>	TTableMap;
		TTableMap										xover::CDealer::s_tables				= {};
		int												xover::CDealer::nextId					= 1;

	// A GET of the dealer resource produces a list of existing tables.
		::nwol::error_t									xover::CDealer::HandleGet								(web::http::http_request message)				{
	ucout <<  message.to_string() << std::endl;

	auto																		paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
	if (paths.empty()) {
		message.reply(web::http::status_codes::OK, TablesAsJSON(s_tables));
		return 0;
	}

	utility::string_t															wtable_id								= paths[0];
	const utility::string_t														table_id								= wtable_id;

	// Get information on a specific table.
	TTableMap::const_iterator													found									= s_tables.find(table_id);
	if (found == s_tables.end())
		message.reply(web::http::status_codes::NotFound);
	else {
		web::json::value															notSure;
		reterr_error_if(errored(found->second->AsJSON(notSure)), "%s", "Failed to get JSON from not sure.");
		message.reply(web::http::status_codes::OK, notSure);
	}
	return 0;
};

// A POST of the dealer resource creates a new table and returns a resource for that table.
		::nwol::error_t									xover::CDealer::HandlePost								(web::http::http_request message)				{
	ucout <<  message.to_string() << std::endl;

	auto																		paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
    
	if (paths.empty()) {
		utility::ostringstream_t													nextIdString;
		nextIdString << nextId;

		std::shared_ptr<CDealerTable>												tbl										= std::make_shared<CDealerTable>(nextId, 8, 6);
		s_tables[nextIdString.str()]											= tbl;
		nextId																	+= 1;

		web::json::value														jsonTable;								reterr_error_if(errored(tbl->AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
		CBJPutResponse															putResponse								= {ST_PlaceBet, jsonTable};
		web::json::value														jsonResponse;							reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
		message.reply(web::http::status_codes::OK, jsonResponse);
		return 0;
	}
	utility::string_t															wtable_id								= paths[0];
	const utility::string_t														table_id								= wtable_id;

	// Join an existing table.
	auto																		found									= s_tables.find(table_id);
	if (found == s_tables.end()) {
		message.reply(web::http::status_codes::NotFound);
		return 0;
	}

	auto																		table									= std::static_pointer_cast<CDealerTable>(found->second);
	if ( table->Players.size() < table->Capacity ) {
		std::map<utility::string_t, utility::string_t>								query									= web::http::uri::split_query(web::http::uri::decode(message.request_uri().query()));
		auto																		cntEntry								= query.find(QUERY_NAME);

		if (cntEntry != query.end() && !cntEntry->second.empty()) {
			table->AddPlayer(CPlayer(cntEntry->second));

			web::json::value															jsonTable;								reterr_error_if(errored(table->AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
			CBJPutResponse																putResponse								= {ST_PlaceBet, jsonTable};
			web::json::value															jsonResponse;							reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
			message.reply(web::http::status_codes::OK, jsonResponse);
		}
		else 
			message.reply(web::http::status_codes::Forbidden, U("Player name is required in query"));
	}
	else {
		utility::ostringstream_t													os;
		os << U("Table ") << table->Id << U(" is full");
		message.reply(web::http::status_codes::Forbidden, os.str());
	}

	return 0;
}

// A DELETE of the player resource leaves the table.
		::nwol::error_t									xover::CDealer::HandleDelete							(web::http::http_request message)				{
	ucout <<  message.to_string() << std::endl;

	auto																		paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
	if (paths.empty()) {
		message.reply(web::http::status_codes::Forbidden, U("TableId is required."));
		return 0;
	}
	utility::string_t															wtable_id								= paths[0];
	const utility::string_t														table_id								= wtable_id;

	// Get information on a specific table.
	auto																		found									= s_tables.find(table_id);
	if (found == s_tables.end()) {
		message.reply(web::http::status_codes::NotFound);
		return 0;
	}
	auto																		table									= std::static_pointer_cast<CDealerTable>(found->second);
	TQueryMap																	query									= web::http::uri::split_query(web::http::uri::decode(message.request_uri().query()));
	auto																		cntEntry								= query.find(QUERY_NAME);
	if ( cntEntry != query.end() ) {
		if ( table->RemovePlayer(cntEntry->second) )
			message.reply(web::http::status_codes::OK);
		else
			message.reply(web::http::status_codes::NotFound);
	}
	else
		message.reply(web::http::status_codes::Forbidden, U("Player name is required in query"));

	return 0;
}

// A PUT to a table resource makes a card request (hit / stay).
		::nwol::error_t									xover::CDealer::HandlePut								(web::http::http_request message)				{
	ucout <<  message.to_string() << std::endl;

	utility::string_t rel_uri_path	= message.relative_uri().path	(); utility::string_t	decoded_paths	= web::http::uri::decode(rel_uri_path	); std::vector<utility::string_t>					paths = web::http::uri::split_path	(decoded_paths);
	utility::string_t rel_uri_query	= message.relative_uri().query	(); utility::string_t	decoded_query	= web::http::uri::decode(rel_uri_query	); std::map<utility::string_t, utility::string_t>	query = web::http::uri::split_query	(decoded_query);
	TQueryMap::const_iterator													queryItr								= query.find(REQUEST);
	if (queryItr == query.end()) {
		message.reply(web::http::status_codes::Forbidden, U("Request is required."));
		error_printf("Invalid request.");
		return 0;
	}
	if (paths.empty() ) {
		message.reply(web::http::status_codes::Forbidden, U("TableId is required."));
		error_printf("Invalid TableId.");
		return 0;
	}

	utility::string_t															wtable_id								= paths[0];
	utility::string_t															request									= queryItr->second;
	const utility::string_t														table_id								= wtable_id;

	// Get information on a specific table.
	TTableMap::const_iterator													found									= s_tables.find(table_id);
	if ( found == s_tables.end() )
		message.reply(web::http::status_codes::NotFound);

	std::shared_ptr<CDealerTable>												table									= std::static_pointer_cast<CDealerTable>(found->second);
		 if ( request == BET		)	table->Bet			(message);
	else if ( request == DOUBLEDOWN	)	table->DoubleDown	(message);
	else if ( request == INSURE		)	table->Insure		(message);
	else if ( request == HIT		)	table->Hit			(message);
	else if ( request == STAY		)	table->Stay			(message);
	else if ( request == REFRESH	)	table->Wait			(message);
	else 
		message.reply(web::http::status_codes::Forbidden, U("Unrecognized request"));

	return 0;
}
