#include "RtcTick.h"
#include <list>

MThreadTick::MThreadTick()
{
}


MThreadTick::~MThreadTick()
{
}

void MThreadTick::RegisteRtcTick(void* ptr, RtcTick* rtcTick)
{
	rtc::CritScope l(&cs_rtc_tick_);
	if (map_rtc_tick_.find(ptr) != map_rtc_tick_.end()) {
		map_rtc_tick_.erase(ptr);
	}

	map_rtc_tick_[ptr] = rtcTick;
}
void MThreadTick::UnRegisteRtcTick(void* ptr)
{
	rtc::CritScope l(&cs_rtc_tick_);
	if (map_rtc_tick_.find(ptr) != map_rtc_tick_.end()) {
		map_rtc_tick_.erase(ptr);
	}
}

void MThreadTick::UnAttachRtcTick(void* ptr)
{
	rtc::CritScope l(&cs_rtc_tick_);
	if (map_rtc_tick_.find(ptr) != map_rtc_tick_.end()) {
		map_rtc_tick_[ptr]->unAttach = true;
	}
}

void MThreadTick::DoProcess()
{
	std::list< RtcTick*> lstUnAttach;
	{
		rtc::CritScope l(&cs_rtc_tick_);
		MapRtcTick::iterator iter = map_rtc_tick_.begin();
		while (iter != map_rtc_tick_.end()) {
			if (iter->second->unAttach) {
				lstUnAttach.push_back(iter->second);
				iter = map_rtc_tick_.erase(iter);
			}
			else {
				iter->second->OnTick();
				iter++;
			}
		}
	}
	std::list< RtcTick*>::iterator itor = lstUnAttach.begin();
	while (itor != lstUnAttach.end()) {
		(*itor)->OnTickUnAttach();
		itor++;
	}
}
