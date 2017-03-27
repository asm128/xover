#include "application.h"
#include "cpprest/http_client.h"
#include "TicTacToe.h"

#ifndef CROSSOVER_CLIENT_H__983264234902
#define CROSSOVER_CLIENT_H__983264234902

struct SApplication : public ::nwol::SApplicationBase {
	utility::string_t									UserName				= U("");
	utility::string_t									Table					= U("");
	web::json::value									AvailableTables			= web::json::value::array();
	bool												WasRefresh				= false;

	::ttt::TicTacToe									Game					= {};
	std::shared_ptr<web::http::client::http_client>		GameClient				= {};

	inline 												SApplication		(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{
		GUI													= {{320, 200}, {::ttt::TicTacToe::SCREEN_WIDTH, ::ttt::TicTacToe::SCREEN_HEIGHT}};
	}
};

int32_t												setup					(::SApplication	& instanceApp);
int32_t												update					(::SApplication	& instanceApp, bool exitRequested);
int32_t												render					(::SApplication	& instanceApp);
int32_t												cleanup					(::SApplication	& instanceApp);

#endif // CROSSOVER_CLIENT_H__983264234902
