#include "xover_server.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "XOver Server", 0, 1);

				::nwol::error_t							CTicTacToeService::HandleGet							(const web::http::http_request& message)				{
	ucout																	<< message.to_string() << std::endl;

	std::vector<utility::string_t>												paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
	if (paths.empty()) {
		web::json::value															jsTables;
		//reterr_error_if(errored(tablesAsJSON(s_tables, jsTables)), "%s", "Failed to convert tables to JSON.");
		message.reply(web::http::status_codes::OK, jsTables);
		return 0;
	}

	utility::string_t															wtable_id								= paths[0];
	const utility::string_t														table_id								= wtable_id;

	// Get information on a specific table.
	//TTableMap::const_iterator													found									= s_tables.find(table_id);
	if (true)//(found == s_tables.end())
		message.reply(web::http::status_codes::NotFound);
	else {
		web::json::value															jsTable;
		//reterr_error_if(errored(found->second->AsJSON(jsTable)), "%s", "Failed to get JSON from table.");
		message.reply(web::http::status_codes::OK, jsTable);
	}
	return 0;
};

// A POST of the dealer resource creates a new table and returns a resource for that table.
				::nwol::error_t							CTicTacToeService::HandlePost							(const web::http::http_request& message)				{
	ucout																	<<  message.to_string() << std::endl;

	std::vector<utility::string_t>												paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
    
	//if (paths.empty()) {
	//	utility::ostringstream_t													nextIdString;
	//	nextIdString << nextId;
	//
	//	std::shared_ptr<CDealerTable>												tbl										= std::make_shared<CDealerTable>(nextId, 8, 6);
	//	s_tables[nextIdString.str()]											= tbl;
	//	nextId																	+= 1;
	//
	//	web::json::value															jsonTable;								reterr_error_if(errored(tbl->AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
	//	CBJPutResponse																putResponse								= {ST_PlaceBet, jsonTable};
	//	web::json::value															jsonResponse;							reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
	//	message.reply(web::http::status_codes::OK, jsonResponse);
	//	return 0;
	//}
	//const utility::string_t														wtable_id								= paths[0];
	//const utility::string_t														table_id								= wtable_id;
	//
	//// Join an existing table.
	//const TTableMap::const_iterator												found									= s_tables.find(table_id);
	//if (found == s_tables.end()) {
	//	message.reply(web::http::status_codes::NotFound);
	//	return 0;
	//}
	//
	//std::shared_ptr<CDealerTable>												table									= std::static_pointer_cast<CDealerTable>(found->second);
	//if ( table->Players.size() < table->Capacity ) {
	//	TQueryMap																	query									= web::http::uri::split_query(web::http::uri::decode(message.request_uri().query()));
	//	TQueryMap::const_iterator													cntEntry								= query.find(QUERY_NAME);
	//
	//	if (cntEntry != query.end() && !cntEntry->second.empty()) {
	//		table->AddPlayer(CPlayer(cntEntry->second));
	//
	//		web::json::value															jsonTable;								reterr_error_if(errored(table->AsJSON(jsonTable)), "%s", "Failed to get JSON from table!");
	//		CBJPutResponse																putResponse								= {ST_PlaceBet, jsonTable};
	//		web::json::value															jsonResponse;							reterr_error_if(errored(putResponse.AsJSON(jsonResponse)), "%s", "Failed to get json from PUT response.");
	//		message.reply(web::http::status_codes::OK, jsonResponse);
	//	}
	//	else 
	//		message.reply(web::http::status_codes::Forbidden, U("Player name is required in query"));
	//}
	//else {
	//	utility::ostringstream_t													os;
	//	os																		<< U("Table ") << table->Id << U(" is full");
	//	message.reply(web::http::status_codes::Forbidden, os.str());
	//}

	return 0;
}

// A DELETE of the player resource leaves the table.
				::nwol::error_t							CTicTacToeService::HandleDelete							(const web::http::http_request& message)				{
	ucout																	<<  message.to_string() << std::endl;

	std::vector<utility::string_t>												paths									= web::http::uri::split_path(web::http::uri::decode(message.relative_uri().path()));
	if (paths.empty()) {
		message.reply(web::http::status_codes::Forbidden, U("TableId is required."));
		return 0;
	}
	utility::string_t															wtable_id								= paths[0];
	const utility::string_t														table_id								= wtable_id;

	// Get information on a specific table.
	//TTableMap::const_iterator													found									= s_tables.find(table_id);
	//if (found == s_tables.end()) {
	//	message.reply(web::http::status_codes::NotFound);
	//	return 0;
	//}
	//std::shared_ptr<CDealerTable>												table									= std::static_pointer_cast<CDealerTable>(found->second);
	//TQueryMap																	query									= web::http::uri::split_query(web::http::uri::decode(message.request_uri().query()));
	//TQueryMap::const_iterator													cntEntry								= query.find(QUERY_NAME);
	//if ( cntEntry != query.end() ) {
	//	if ( table->RemovePlayer(cntEntry->second) )
	//		message.reply(web::http::status_codes::OK);
	//	else
	//		message.reply(web::http::status_codes::NotFound);
	//}
	//else
	//	message.reply(web::http::status_codes::Forbidden, U("Player name is required in query"));

	return 0;
}

// A PUT to a table resource makes a card request (hit / stay).
				::nwol::error_t							CTicTacToeService::HandlePut							(const web::http::http_request& message)				{
	ucout																	<<  message.to_string() << std::endl;

	utility::string_t rel_uri_path	= message.relative_uri().path	(); utility::string_t	decoded_paths	= web::http::uri::decode(rel_uri_path	); std::vector<utility::string_t>					paths = web::http::uri::split_path	(decoded_paths);
	utility::string_t rel_uri_query	= message.relative_uri().query	(); utility::string_t	decoded_query	= web::http::uri::decode(rel_uri_query	); std::map<utility::string_t, utility::string_t>	query = web::http::uri::split_query	(decoded_query);
	//TQueryMap::const_iterator													queryItr								= query.find(REQUEST);
	//if (queryItr == query.end()) {
	//	message.reply(web::http::status_codes::Forbidden, U("Request is required."));
	//	error_printf("Invalid request.");
	//	return 0;
	//}
	//if (paths.empty() ) {
	//	message.reply(web::http::status_codes::Forbidden, U("TableId is required."));
	//	error_printf("Invalid TableId.");
	//	return 0;
	//}
	//
	//utility::string_t															wtable_id								= paths[0];
	//utility::string_t															request									= queryItr->second;
	//const utility::string_t														table_id								= wtable_id;
	//
	//// Get information on a specific table.
	//TTableMap::const_iterator													found									= s_tables.find(table_id);
	//if ( found == s_tables.end() )
	//	message.reply(web::http::status_codes::NotFound);
	//
	//std::shared_ptr<CDealerTable>												table									= std::static_pointer_cast<CDealerTable>(found->second);
	//	 if ( request == BET		)	table->Bet			(message);
	//else if ( request == DOUBLEDOWN	)	table->DoubleDown	(message);
	//else if ( request == INSURE		)	table->Insure		(message);
	//else if ( request == HIT		)	table->Hit			(message);
	//else if ( request == STAY		)	table->Stay			(message);
	//else if ( request == REFRESH	)	table->Wait			(message);
	//else 
	//	message.reply(web::http::status_codes::Forbidden, U("Unrecognized request"));

	return 0;
}


static inline	void						shutdown_listener					(::SApplication& instanceApp)																{ instanceApp.GameHost.Close().wait(); }
				int32_t						init_listener						(::SApplication& instanceApp, const utility::string_t& address)								{
    // Build our listener's URI from the configured address and the hard-coded path "blackjack/dealer"
    web::uri_builder								uri										= address;
    uri.append_path(U("blackjack/dealer"));

    const utility::string_t							addr									= uri.to_uri().to_string();
	instanceApp.GameHost							= ::CTicTacToeService(addr);
	instanceApp.GameHost.Open().wait();

	char_t											proper_address[128]	= {};
	size_t											n					= 0;
	wcstombs_s(&n, proper_address, addr.c_str(), addr.size());
	debug_printf("Listening for requests at: %s", proper_address);
    return 0;
}

				int32_t						setupREST						(::SApplication& instanceApp)																	{ 
    utility::string_t								port									= U("34568");
    if(3 <= instanceApp.RuntimeValues->CommandLineArgCount) {
#if defined(__WINDOWS__)
		wchar_t											temp[8]									= {};
		size_t											numCharsConverted						= 0;
		mbstowcs_s(&numCharsConverted, temp, instanceApp.RuntimeValues->CommandLineArgList[2], 5);
		port										= temp;
#else
		port										= instanceApp.RuntimeValues->CommandLineArgList[2];
#endif
	}
    utility::string_t								address									= U("http://localhost:");
    address.append(port);

	return init_listener(instanceApp, address);
}

				int32_t						setupGUI						(::SApplication& instanceApp)																	{ 
	::nwol::SGUI									& guiSystem								= instanceApp.GUI;

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
				int32_t						cleanup							(::SApplication& instanceApp)																	{
	::nwol::shutdownASCIIScreen();

	::shutdown_listener(instanceApp);
	return 0; 
}

				int32_t						setup							(::SApplication& instanceApp)																	{ 
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

				int32_t						update							(::SApplication& instanceApp, bool exitRequested)												{
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
	sizeof(ttt::TicTacToeBoard16);
	sizeof(ttt::TicTacToeBoard32);
	sizeof(ttt::CellCoord);
	sizeof(ttt::CellPick);
	return 0; 
}

template<uint32_t _screenWidth, uint32_t _screenHeight>
				void						bltASCIIScreen					(const ttt::ScreenASCII<_screenWidth, _screenHeight>& source, ::nwol::SASCIITarget& target)		{
	for(uint32_t y = 0, yMax = ::nwol::min(_screenHeight, target.Height()); y<yMax; ++y)
		for(uint32_t x = 0, xMax = ::nwol::min(_screenWidth, target.Width()); x<xMax; ++x)
			target.Text[y][x]							= source.Cells[y][x];
}

				int32_t						render							(::SApplication& instanceApp)																	{ 
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
