#include <glib.h>
#include <gio/gio.h>
#include <iostream>

#include "file-metadata-extractor.h"
#include "taglib-metadata-extractor.h"
#include "device-manager.h"
#include "logging.h"
#include "console-logging.h"


// emulate new device inserted
gboolean newDeviceAdded( gpointer ptr)
{
	DeviceManager::getInstance().handleDeviceInserted("BDB4-A56F_2", "/media/BDB4-A56F");
	return FALSE;
}

int main( int argc, char * argv[])
{
	GMainLoop *loop = g_main_loop_new ( NULL , FALSE );

	// initialize extractors
	// we use one single extractor type for all audio types

	TagLibMetadataExtractor taglibExtractor;
	MetadataExtractManager::getInstance().registerExtractor( "MP3",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "WAV",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AAC",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "M4A",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "MP4",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "FLAC", &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "WMA",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AMR",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "AWB",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "OGG",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "OGA",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "MP2",  &taglibExtractor);
	MetadataExtractManager::getInstance().registerExtractor( "M4B",  &taglibExtractor);



    gchar * deviceId = NULL;
    gchar * devicePath = NULL;



    GOptionEntry options[] =
    {
        { "device-id",     'i', 0, G_OPTION_ARG_STRING, &deviceId, "deviceID ", NULL },
        { "device-path",   'p', 0, G_OPTION_ARG_STRING, &devicePath, "devicePath ", NULL },
        { NULL }
    };

    GOptionContext *optionContext = g_option_context_new(NULL);
    g_option_context_add_main_entries(optionContext, options, NULL);

    GError *error = NULL;
    if( !g_option_context_parse(optionContext, &argc, &argv, &error)) {
        std::cout << "Option parsing failed: " <<  error->message << std::endl;
        return 1;
    }

    if( deviceId == NULL )
    {
    	std::cout << "Invalid device id" << std::endl;
    	return -2;
    }

    if( devicePath == NULL )
    {
    	std::cout << "Invalid device path" << std::endl;
    	return -3;
    }

    if(!g_thread_supported())
    	g_thread_init (NULL);


    g_type_init();

    Logging::Logger::getInstance().setLogLevel( LOG_VERBOSE );
    Logging::Logger::getInstance().addLoggerDelegate( LoggerDelegate( ConsoleLoggerMethod::getInstance() ) );


    // this method should called when usb stick inserted
	// see Linux 'mount' or 'ls /media' for more details, usually the mount point differ
	// we emulate this with hard-coded test values
    DeviceManager::getInstance().handleDeviceInserted(deviceId, devicePath);

    //DeviceManager::getInstance().handleDeviceInserted("BDB4-A56F", "/media/BDB4-A56F");
	/*
	 *  optionally we can emulate other device inserted
	 * the current scan should be canceled
	 * and another one must be started
	 * another database with different name will be created
	 */

	//g_timeout_add_seconds( 10, newDeviceAdded, loop );


	g_main_loop_run (loop);
	g_main_loop_unref (loop);

	return 0;
}

