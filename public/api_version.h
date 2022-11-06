#ifndef API_VERSION_H
#define API_VERSION_H

#ifdef _WIN32
#pragma once
#endif

typedef struct api_version_s
{
	int major_version;
	int minor_version;
} api_version_t;

#endif // API_VERSION_H