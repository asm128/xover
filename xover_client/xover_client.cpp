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

#ifdef _WIN32
# define iequals(x, y) (_stricmp((x), (y))==0)
#else
# define iequals(x, y) boost::iequals((x), (y))
#endif

static inline	web::http::http_response		checkResponse						(const std::string &url, const web::http::http_response &response)							{
    ucout << response.to_string() << std::endl;	
    return response;
}

web::http::http_response						checkResponse						(const std::string &url, const web::http::http_response &response, bool &refresh)			{
    ucout << response.to_string() << std::endl;
    ::xover::CBJPutResponse								answer;
	::xover::FromJSON(response.extract_json().get(), answer);
    refresh											= answer.Status == ::xover::ST_Refresh;
    return response;
}

void											printResult							(::xover::CBJHand::EBJHandResult result)													{
    switch (result) {
    case ::xover::CBJHand::HR_PlayerBlackJack	: ucout << "Black Jack"		; break;
    case ::xover::CBJHand::HR_PlayerWin			: ucout << "Player wins"	; break;
    case ::xover::CBJHand::HR_ComputerWin		: ucout << "Computer Wins"	; break;
    case ::xover::CBJHand::HR_Push				: ucout << "Push"			; break;
    }
}

void											printCard							(const ::xover::SCard &card)																{
    switch (card.Value) {
    case ::xover::SCard::CV_King	: ucout << "K"; break;
    case ::xover::SCard::CV_Queen	: ucout << "Q"; break;
    case ::xover::SCard::CV_Jack	: ucout << "J"; break;
    case ::xover::SCard::CV_Ace		: ucout << "A"; break;
    default: ucout << (int)card.Value; break;
    }
    switch (card.Suit) {
    case ::xover::SCard::CS_Club	: ucout << "C"; break;
    case ::xover::SCard::CS_Spade	: ucout << "S"; break;
    case ::xover::SCard::CS_Heart	: ucout << "H"; break;
    case ::xover::SCard::CS_Diamond	: ucout << "D"; break;
    }
}

void											printHand						(bool suppress_bet, const ::xover::CBJHand &hand)												{
    if (!suppress_bet) {
        if ( hand.Insurance > 0 )
            ucout											<< "Bet: " << hand.Bet << "Insurance: " << hand.Insurance << " Hand: ";
        else
            ucout											<< "Bet: " << hand.Bet << " Hand: ";
    }
    for (auto iter = hand.Cards.begin(); iter != hand.Cards.end(); iter++) {
        printCard(*iter); 
		ucout << " ";
	}
    printResult(hand.Result);
}
//-----------------------------
void											printTable						(const web::http::http_response &response, bool &refresh)										{
    ::xover::CBJHand									hand;

    refresh											= false;

    if ( response.status_code() == web::http::status_codes::OK ) {
        if ( response.headers().content_type() == U("application/json") ) {
            ::xover::CBJPutResponse								answer;
			FromJSON(response.extract_json().get(), answer);
            web::json::value									players							= answer.Data[PLAYERS];

            refresh											= answer.Status == ::xover::ST_Refresh;

            for (auto iter = players.as_array().begin(); iter != players.as_array().end(); ++iter) {
                auto												& player						= *iter;
                web::json::value									name							= player[NAME];
                web::json::value									bet								= player[BALANCE];
                bool												suppressMoney					= iter == players.as_array().begin();
                if ( suppressMoney )
                    ucout											<< "'" << name.as_string() << "'" ;
                else
                    ucout											<< "'" << name.as_string() << "' Balance = $" << bet.as_double() << " ";

				::xover::CBJHand									hand;
				::xover::FromJSON(player[HAND].as_object(), hand);
                printHand(suppressMoney, hand);
                ucout											<< std::endl;
            }

            switch ( answer.Status ) {
            case ::xover::ST_PlaceBet: ucout					<< "Place your bet!\n"	; break;
            case ::xover::ST_YourTurn: ucout					<< "Your turn!\n"		; break;
            }
        }
    }
}
//-----------------------------
::nwol::error_t									tickTable						(::SApplication& instanceApp)																	{
	web::http::client::http_client						& bjDealer						= *instanceApp.BJDealer.get();
	web::json::value									& availableTables				= instanceApp.AvailableTables;
	bool												& wasRefresh					= instanceApp.WasRefresh;
	utility::string_t									& userName						= instanceApp.UserName;
	utility::string_t									& tableName						= instanceApp.Table;

    while ( wasRefresh ) {
        wasRefresh									= false;
        utility::ostringstream_t						buf;
        buf											<< tableName << U("?request=refresh&name=") << userName;
        printTable(checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
    }

	std::string											method;
    ucout											<< "Enter method:";
    std::cin										>> method;
    if ( iequals(method.c_str(), "quit") ) {
        if ( !userName.empty() && !tableName.empty() ) {
            utility::ostringstream_t							buf;
            buf												<< tableName << U("?name=") << userName;
            checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::DEL, buf.str()).get());
        }
        return 1;
    }
    if ( iequals(method.c_str(), "name") ) {
        ucout											<< "Enter user name:";
        ucin											>> userName;
    }
    else if ( iequals(method.c_str(), "join") ) {
        ucout											<< "Enter table name:";
        ucin											>> tableName;
        if ( userName.empty() ) { 
			ucout											<< "Must have a name first!\n"; 
			return 0; 
		}
        utility::ostringstream_t							buf;
        buf												<< tableName << U("?name=") << userName;
        checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::POST, buf.str()).get(), wasRefresh);
    }
    else if(iequals(method.c_str(), "hit"	)
        ||	iequals(method.c_str(), "stay"	)
        ||	iequals(method.c_str(), "double") 
		)
    {
        utility::ostringstream_t							buf;
        buf												<< tableName << U("?request=") << utility::conversions::to_string_t(method) << U("&name=") << userName;
        printTable(checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
    }
    else if(iequals(method.c_str(), "bet"	)	 
		||	iequals(method.c_str(), "insure") 
		)
    {
        utility::string_t									bet;
        ucout											<< "Enter bet:";
        ucin											>> bet;

        if ( userName.empty() ) { 
			ucout											<< "Must have a name first!\n"; 
			return 0; 
		}

        utility::ostringstream_t							buf;
        buf												<< tableName << U("?request=") << utility::conversions::to_string_t(method) << U("&name=") << userName << U("&amount=") << bet;
        printTable(checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::PUT, buf.str()).get()), wasRefresh);
    }
    else if ( iequals(method.c_str(), "newtbl") )
        checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::POST).get(), wasRefresh);
    else if ( iequals(method.c_str(), "leave") ) {
        ucout											<< "Enter table:";
        ucin											>> tableName;

        if ( userName.empty() ) { 
			ucout											<< "Must have a name first!\n"; 
			return 0; 
		}

        utility::ostringstream_t							buf;
        buf												<< tableName << U("?name=") << userName;
        checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::DEL, buf.str()).get(), wasRefresh);
    }
    else if ( iequals(method.c_str(), "list") ) {
        wasRefresh										= false;
        web::http::http_response							response						= checkResponse("blackjack/dealer", bjDealer.request(web::http::methods::GET).get());

        if ( response.status_code() == web::http::status_codes::OK ) {
            availableTables										= response.extract_json().get();
            for (auto iter = availableTables.as_array().begin(); iter != availableTables.as_array().end(); ++iter) {
                ::xover::CBJTable									bj_table;
				::xover::FromJSON(iter->as_object(), bj_table);
                web::json::value									id								= web::json::value::number(bj_table.Id);
                ucout											<< "table " << bj_table.Id << ": {capacity: " << (long unsigned int)bj_table.Capacity << " no. players: " << (long unsigned int)bj_table.Players.size() << " }\n";
            }
            ucout											<< std::endl;
        }
    }
    else {
        ucout											<< utility::conversions::to_string_t(method) << ": not understood\n";
    }

    return 0;
}
//-----------------------------
static inline	void							client_shutdown							(::SApplication& instanceApp)															{}
				int32_t							client_init								(::SApplication& instanceApp, const utility::string_t& address)							{
	web::http::uri										uri										= web::http::uri(address);
	instanceApp.BJDealer							=  std::make_shared<web::http::client::http_client>( web::http::uri_builder(uri).append_path(U("/blackjack/dealer")).to_uri() );

	instanceApp.AvailableTables						= web::json::value::array();
    instanceApp.WasRefresh							= false;

	return 0;
}

GDEFINE_ENUM_TYPE(CLIENT_ACTION, uint8_t);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x00, state		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x01, bet			);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x02, double		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x03, insure		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x04, hit			);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x05, stay		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x06, refresh		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x07, insurance	);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x08, result		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x09, suit		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0A, Name		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0B, Balance		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0C, Hand		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0D, value		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0E, cards		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x0F, Capacity	);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x10, Id			);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x11, Players		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x12, DEALER		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x13, Data		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x14, Status		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x15, request		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x16, amount		);
GDEFINE_ENUM_VALUE(CLIENT_ACTION, 0x17, name		);
										
//-----------------------------			
int32_t											setupREST								(::SApplication& instanceApp)															{ 
	utility::string_t									port									= U("34568");
    if(3 <= instanceApp.RuntimeValues->CommandLineArgCount) {
#if defined(__WINDOWS__)
		wchar_t												temp[8]									= {};
		size_t												numCharsConverted						= 0;
		mbstowcs_s(&numCharsConverted, temp, instanceApp.RuntimeValues->CommandLineArgList[2], 5);
		port											= temp;
#else
		port											= instanceApp.RuntimeValues->CommandLineArgList[2];
#endif
	}
    utility::string_t									address									= U("http://localhost:");
    address.append(port);

	return client_init(instanceApp, address);
}
//-----------------------------
int32_t											setupGUI								(::SApplication& instanceApp)															{ 
	::nwol::SGUI										& guiSystem								= instanceApp.GUI;

	guiSystem.TargetSizeASCII.x						= ::nwol::getASCIIBackBufferWidth	();
	guiSystem.TargetSizeASCII.y						= ::nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel							newControlLabel							= "Exit";
	::nwol::SGUIControl									newControl;

	newControl.AreaASCII							= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text									= newControlLabel								;

	::nwol::error_t										errMy									= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "Failed to create control!");

	newControl.AreaASCII							= {1, 0, 1, 1};
	newControl.AlignArea							= ::nwol::SCREEN_TOP_LEFT;
	for(uint32_t iButton=1, buttonCount = ::nwol::genum_definition<CLIENT_ACTION>::get().Names.size(); iButton < buttonCount; ++iButton) {
		newControl.Text									= ::nwol::genum_definition<CLIENT_ACTION>::get().Names[iButton];
		newControl.AreaASCII.Size.x						= newControl.Text.size();
		++newControl.AreaASCII.Offset.y;
		::nwol::createControl(guiSystem, newControl);
	}

	return 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int32_t											cleanup									(::SApplication& instanceApp)															{
	::nwol::shutdownASCIIScreen();

	::client_shutdown(instanceApp);
	return 0; 
}

int32_t											setup									(::SApplication& instanceApp)															{ 
	::nwol::SCoord2<uint32_t>							& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	::nwol::setASCIIScreenTitle(appTitle());

	::nwol::error_t									
	errMy											= ::setupREST		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup REST."		);
	errMy											= ::setupGUI		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup GUI."			);
	//errMy											= ::networkEnable	(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to enable network."	);
	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

int32_t											update									(::SApplication& instanceApp, bool exitRequested)										{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput										& inputSystem							= instanceApp.Input;
	::nwol::pollInput(inputSystem);

	const ::nwol::SInput								& inputSystemConst						= instanceApp.Input;
	::nwol::SGUI										& guiSystem								= instanceApp.GUI;
	
	::nwol::error_t										errMy									= ::nwol::updateGUI(guiSystem, inputSystemConst);
	reterr_error_if_errored(errMy, "%s", "Failed to update GUI.");
	
	::nwol::array_pod<::nwol::CONTROL_FLAG>				& controlFlags							= guiSystem.Controls.ControlFlags;

	for(uint32_t iControl = 0, controlCount = controlFlags.size(); iControl < controlCount; ++iControl)
		if(::nwol::bit_true(controlFlags[iControl], ::nwol::CONTROL_FLAG_EXECUTE)) {
			debug_printf("Execute control %u.", iControl);
			switch(iControl) {
			case 0:		
				return ::nwol::APPLICATION_STATE_EXIT; 
				break;
			default:
				break;
			}
		}
	return 0; 
}

int32_t											render									(::SApplication& instanceApp)															{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget								target;
	::nwol::error_t									
	errMy											= ::nwol::getASCIIBackBuffer(target);				reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	errMy											= ::nwol::renderGUIASCII(target, instanceApp.GUI);	reterr_error_if_errored(errMy, "%s", "Failed to render GUI to ASCII target!");

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}
