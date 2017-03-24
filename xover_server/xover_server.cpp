#include "xover_server.h"

#include "text.h"
#include "ascii_screen.h"
#include "gui.h"

#include "runtime_impl.h"

#include <thread>
#include <process.h>

DEFINE_RUNTIME_INTERFACE_FUNCTIONS(::SApplication, "XOver Server", 0, 1);

#include "Dealer.h"

static inline void							shutdown_listener						(::SApplication& instanceApp)											{ instanceApp.Dealer.Close().wait(); }
int32_t										init_listener							(::SApplication& instanceApp, const utility::string_t& address)			{
    // Build our listener's URI from the configured address and the hard-coded path "blackjack/dealer"
    web::uri_builder								uri										= address;
    uri.append_path(U("blackjack/dealer"));

    const utility::string_t							addr									= uri.to_uri().to_string();
	instanceApp.Dealer							= xover::CDealer(addr);
	instanceApp.Dealer.Open().wait();

	char_t											proper_address[128]	= {};
	size_t											n					= 0;
	wcstombs_s(&n, proper_address, addr.c_str(), addr.size());
	debug_printf("Listening for requests at: %s", proper_address);
    return 0;
}

int32_t										setupREST								(::SApplication& instanceApp)											{ 
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

int32_t										setupGUI								(::SApplication& instanceApp)											{ 
	::nwol::SGUI									& guiSystem								= instanceApp.GUI;

	guiSystem.TargetSizeASCII.x					= ::nwol::getASCIIBackBufferWidth	();
	guiSystem.TargetSizeASCII.y					= ::nwol::getASCIIBackBufferHeight	();

	static const ::nwol::glabel						newControlLabel							= "Exit";
	::nwol::SGUIControl								newControl;

	newControl.AreaASCII						= {1, 1, (int32_t)newControlLabel.size(), 1}	;
	newControl.Text								= newControlLabel								;

	::nwol::error_t									errMy									= ::nwol::createControl(guiSystem, newControl);
	reterr_error_if_errored(errMy, "Failed to create control!");

	return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
int32_t										cleanup									(::SApplication& instanceApp)											{
	::nwol::shutdownASCIIScreen();

	::shutdown_listener(instanceApp);
	return 0; 
}

int32_t										setup									(::SApplication& instanceApp)											{ 
	::nwol::SCoord2<uint32_t>						& sizeTargetASCII						= instanceApp.GUI.TargetSizeASCII;
	::nwol::initASCIIScreen(sizeTargetASCII.x, sizeTargetASCII.y);
	::nwol::setASCIIScreenTitle(appTitle());

	::nwol::error_t									
	errMy										= ::setupREST		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup REST."		);
	errMy										= ::setupGUI		(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to setup GUI."			);
	//errMy										= ::networkEnable	(instanceApp);	reterr_error_if_errored(errMy, "%s", "Failed to enable network."	);
	return 0; 
}

#ifndef VK_ESCAPE
#define VK_ESCAPE 0x1B
#endif

int32_t										update									(::SApplication& instanceApp, bool exitRequested)						{
	if(exitRequested)
		return ::nwol::APPLICATION_STATE_EXIT;

	::nwol::SInput									& inputSystem							= instanceApp.Input;
	::nwol::pollInput(inputSystem);

	const ::nwol::SInput							& inputSystemConst						= instanceApp.Input;
	::nwol::SGUI									& guiSystem								= instanceApp.GUI;
	
	::nwol::error_t									errMy									= ::nwol::updateGUI(guiSystem, inputSystemConst);
	reterr_error_if_errored(errMy, "%s", "Failed to update GUI.");
	
	::nwol::array_pod<::nwol::CONTROL_FLAG>			& controlFlags							= guiSystem.Controls.ControlFlags;

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

int32_t										render									(::SApplication& instanceApp)											{ 
	::nwol::clearASCIIBackBuffer(' ', COLOR_WHITE);

	::nwol::SASCIITarget							target;
	::nwol::error_t									
	errMy										= ::nwol::getASCIIBackBuffer(target);				reterr_error_if_errored(errMy, "%s", "Failed to get ASCII target!");
	errMy										= ::nwol::renderGUIASCII(target, instanceApp.GUI);	reterr_error_if_errored(errMy, "%s", "Failed to render GUI to ASCII target!");

	::nwol::presentASCIIBackBuffer();
	
	return 0; 
}
