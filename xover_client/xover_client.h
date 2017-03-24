#include "application.h"
#include "cpprest/http_client.h"

#ifndef CROSSOVER_SERVER_H__983264902
#define CROSSOVER_SERVER_H__983264902

struct SApplication : public ::nwol::SApplicationBase 
{
	std::shared_ptr<web::http::client::http_client>		BJDealer;
	utility::string_t									UserName;
	utility::string_t									Table;
	web::json::value									AvailableTables			= web::json::value::array();
	bool												WasRefresh				= false;

	inline 												SApplication			(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{}
};

int32_t												setup					(::SApplication	& instanceApp);
int32_t												update					(::SApplication	& instanceApp, bool exitRequested);
int32_t												render					(::SApplication	& instanceApp);
int32_t												cleanup					(::SApplication	& instanceApp);

#endif // CROSSOVER_SERVER_H__983264902
