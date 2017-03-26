#include "application.h"
#include "Dealer.h"
#include "TicTacToe.h"

#ifndef CROSSOVER_SERVER_H__983264902
#define CROSSOVER_SERVER_H__983264902

// Contains the main logic of the black jack dealer.
class CTicTacToeService {
	::nwol::array_pod<::ttt::TicTacToe>								Games							;
	static int														nextId							;

			web::http::experimental::listener::http_listener		Listener						;   

			::nwol::error_t											HandleGet						(const web::http::http_request& message);
			::nwol::error_t											HandlePut						(const web::http::http_request& message);
			::nwol::error_t											HandlePost						(const web::http::http_request& message);
			::nwol::error_t											HandleDelete					(const web::http::http_request& message);
public:
																	CTicTacToeService				()									{}
																	CTicTacToeService				(utility::string_t url)				: Listener(url)				{
		Listener.support(web::http::methods::GET	, std::bind(&CTicTacToeService::HandleGet		, this, std::placeholders::_1));
		Listener.support(web::http::methods::PUT	, std::bind(&CTicTacToeService::HandlePut		, this, std::placeholders::_1));
		Listener.support(web::http::methods::POST	, std::bind(&CTicTacToeService::HandlePost		, this, std::placeholders::_1));
		Listener.support(web::http::methods::DEL	, std::bind(&CTicTacToeService::HandleDelete	, this, std::placeholders::_1));

		Games.push_back({});
	}
	inline	pplx::task<void>										Open							()									{ return Listener.open	(); }
	inline	pplx::task<void>										Close							()									{ return Listener.close	(); }
};

struct SApplication : public ::nwol::SApplicationBase {
	::xover::CDealer									Dealer;
	::ttt::TicTacToe									Game;

	inline 												SApplication		(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI													= {{320, 200}, {::ttt::TicTacToe::SCREEN_WIDTH, ::ttt::TicTacToe::SCREEN_HEIGHT}};
	}
};

int32_t												setup				(::SApplication	& instanceApp);
int32_t												update				(::SApplication	& instanceApp, bool exitRequested);
int32_t												render				(::SApplication	& instanceApp);
int32_t												cleanup				(::SApplication	& instanceApp);

#endif // CROSSOVER_SERVER_H__983264902
