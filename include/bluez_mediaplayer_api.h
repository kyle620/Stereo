/**
	* @file bluez_mediaplayer_api.h
	* @author Kyle Van Cleave
	* @date March 14, 2020
	* 
	* @brief This file implements majority of the functioanlity described in the "media-api.txt" file provided by bluez.
	* To learn more, please visit 'https://git.kernel.org/pub/scm/bluetooth/bluez.git/tree/doc/media-api.txt'.
**/

#ifndef BLUEZMEDIAPLAYER_H
#define BLUEZMEDIAPLAYER_H


#include <glib.h>
#include <gio/gio.h>
#include <stdbool.h>

/**
	*@brief Defines for the properties we can ask for according to the org.bluez.MediaPlayer1 interface
**/
#define PROPERTY_REPEAT			"Repeat"		/**< Boolean */
#define PROPERTY_SHUFFLE		"Shuffle"		/**< Boolean */
#define PROPERTY_STATUS			"Status"		/**< String */
#define PROPERTY_NAME			"Name"			/**< String */
#define PROPERTY_TYPE			"Type"			/**< String */
#define PROPERTY_POSITION		"Position"		/**< UINT32 */
#define PROPERTY_DURATION 		"Duration"		/**< UINT32 */
#define PROPERTY_TRACK_NUMBER	"TrackNumber"	/**< UINT32 */
#define PROPERTY_TRACK_TITLE	"Title"			/**< String */
#define PROPERTY_TRACK_ARTIST	"Artist"		/**< String */
#define PROPERTY_TRACK_ALBUM	"Album"			/**< String */
#define PROPERTY_TRACK_GENRE	"Genre"			/**< String */

 /**
	 * TODO need to do bound checking on size of chars
	 * - Currenlty most properites inside MediaPlayer have set array sizes for strings
	 *	 such as player path, player name, track title, album title, ..., etc 
	 * 	 the set size has possiblity of being exceeded
**/

/* 
 *	According to bluez doc 'media-api.txt' here are the properties for a MediaPlayer1

 * Properties	string Equalizer [readwrite]

			Possible values: "off" or "on"

		string Repeat [readwrite]

			Possible values: "off", "singletrack", "alltracks" or
					"group"

		string Shuffle [readwrite]

			Possible values: "off", "alltracks" or "group"

		string Scan [readwrite]

			Possible values: "off", "alltracks" or "group"

		string Status [readonly]

			Possible status: "playing", "stopped", "paused",
					"forward-seek", "reverse-seek"
					or "error"

		uint32 Position [readonly]

			Playback position in milliseconds. Changing the
			position may generate additional events that will be
			sent to the remote device. When position is 0 it means
			the track is starting and when it's greater than or
			equal to track's duration the track has ended. Note
			that even if duration is not available in metadata it's
			possible to signal its end by setting position to the
			maximum uint32 value.

		dict Track [readonly]

			Track metadata.

			Possible values:

				string Title:

					Track title name

				string Artist:

					Track artist name

				string Album:

					Track album name

				string Genre:

					Track genre name

				uint32 NumberOfTracks:

					Number of tracks in total

				uint32 TrackNumber:

					Track number

				uint32 Duration:

					Track duration in milliseconds

		object Device [readonly]

			Device object path.

		string Name [readonly]

			Player name

		string Type [readonly]

			Player type

			Possible values:

				"Audio"
				"Video"
				"Audio Broadcasting"
				"Video Broadcasting"

		string Subtype [readonly]

			Player subtype

			Possible values:

				"Audio Book"
				"Podcast"

		boolean Browsable [readonly]

			If present indicates the player can be browsed using
			MediaFolder interface.

			Possible values:

				True: Supported and active
				False: Supported but inactive

			Note: If supported but inactive clients can enable it
			by using MediaFolder interface but it might interfere
			in the playback of other players.


		boolean Searchable [readonly]

			If present indicates the player can be searched using
			MediaFolder interface.

			Possible values:

				True: Supported and active
				False: Supported but inactive

			Note: If supported but inactive clients can enable it
			by using MediaFolder interface but it might interfere
			in the playback of other players.

		object Playlist

			Playlist object path.
*/
struct _MediaPlayer{
	char	OBJECT_PATH[100];	/**< Path of the device we are connected to: example: /org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX */
	char 	PLAYER_PATH[100];	/**< Path of the player we want to listen/exchange track data with, and control using control methods */
	char	REPEAT[20];			/**< Possible values: "off", "singletrack", "alltracks" or "group" */
	char	SHUFFLE[20];		/**< Possible values: "off", "alltracks" or "group" */
	char	STATUS[20];			/**< Possible status: "playing", "stopped", "paused", "forward-seek", "reverse-seek" or "error" */
	char	PLAYER_NAME[100];	/**< This is the name of the player, example: Spotify, Apple Music, Amazon Music */
	char	TYPE[30];			/**< Possible values: "Audio" "Video" "Audio Broadcasting" "Video Broadcasting" */
	char	TRACK_TITLE[100];	/**< Track title Name */
	char	TRACK_ARTIST[100];	/**< Artist of track */
	char	TRACK_ALBUM[100];	/**< Album of track */
	char	TRACK_GENRE[100];	/**< Genre of track */
	bool	CONNECTED;			/**< True if media player exists */
	guint32 TRACK_NUMBER;		/**< Current Track Number */
	guint32	TRACK_DURATION;		/**< Track Duration in ms */
	guint32 TRACK_POSITION;		/**< Current Track Position */
};

typedef struct _MediaPlayer MediaPlayer;

/* 
*	Accessors
*/
void bluez_media_player_print_current_player(void);							// prints the properties of the current player

/*
* Modifiers
*/
int bluez_media_player_init(GDBusConnection * conn);
void bluez_media_player_set_default_properties(void);
void bluez_media_player_init_signals(void);
void bluez_media_player_mute_signals(void);
void bluez_media_player_read_remote_player_properties();

/*
* Control Methods
*/

/*
Methods		void Play()

			Resume playback.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void Pause()

			Pause playback.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void Stop()

			Stop playback.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void Next()

			Next item.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void Previous()

			Previous item.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void FastForward()

			Fast forward playback, this action is only stopped
			when another method in this interface is called.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed

		void Rewind()

			Rewind playback, this action is only stopped
			when another method in this interface is called.

			Possible Errors: org.bluez.Error.NotSupported
					 org.bluez.Error.Failed
*/
void bluez_mediaplayer_play();					// Tells remote object to start playing track
void bluez_mediaplayer_pause();					// Tells remote object to pause
void bluez_mediaplayer_stop();					// Tells remote object to stop
void bluez_mediaplayer_next();					// Tells remote object to skip to next track
void bluez_mediaplayer_previous();				// Tells remote object to jump back to previous track
void bluez_mediaplayer_shuffle_on();			// Tells remote object to turn shuffle 'alltracks'
void bluez_mediaplayer_shuffle_off();			// Tells remote object to turn shuffle off
void bluez_mediaplayer_repeat_singletrack();	// Tells remote object to turn repeat 'alltracks'
void bluez_mediaplayer_repeat_alltracks();		// Tells remote object to turn repeat 'alltracks'
void bluez_mediaplayer_repeat_off();			// Tells remote object to turn repeat 'off'


#endif
