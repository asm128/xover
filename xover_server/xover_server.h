#include "application.h"
#include "Dealer.h"

#ifndef CROSSOVER_SERVER_H__983264902
#define CROSSOVER_SERVER_H__983264902

struct SApplication : public ::nwol::SApplicationBase 
{
	xover::CDealer				Dealer;

	inline 						SApplication	(::nwol::SRuntimeValues	* runtimeValues)	: SApplicationBase(runtimeValues)	{}
};

int32_t						setup			(::SApplication	& instanceApp);
int32_t						update			(::SApplication	& instanceApp, bool exitRequested);
int32_t						render			(::SApplication	& instanceApp);
int32_t						cleanup			(::SApplication	& instanceApp);

#endif // CROSSOVER_SERVER_H__983264902
