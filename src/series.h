//
// Created by moss on 9/29/22.
//
#pragma once

#include <iostream>
#include <vector>

#include "season.h"

namespace dropout_dl {


	/// A class for handling all series information and functions.
	class series {
		public:
			/// The name of the series
			std::string name;
			/// The link to the series page
			std::string url;
			/// The series page data
			std::string page_data;
			/// The directory which will contain the seasons of the series
			std::string series_directory;
			/// A vector containing all the season that this series include
			std::vector<season> seasons;
			/// A vector containing the cookies needed to download episodes
			dropout_dl::cookie session_cookie;
			/// Whether or not to download captions
			bool download_captions;
			/// Whether to skip the video and only download captions
			bool download_captions_only;
			/// Ammount of time between downloading episodes
			uint32_t rate_limit;

			/**
			 *
			 * @param html_data - The series page data
			 * @return The name of the series
			 *
			 * Scrapes the series page for the name of the series
			 */
			static std::string get_series_name(const std::string& html_data);

			/**
			 *
			 * Scrapes the series page for the names and link of all the season. Creates season objects for each of these.
			 * These season object contain all the episodes of the season as episode objects.
			 * The cookies this function takes are passed to the episode objects.
			 */
			std::vector<season> get_seasons();

			/**
			 *
			 * @param url - The url to the season
			 * @param session_cookie - The browser cookie
			 * @param download_captions_only - Whether or not to only download captions
			 * @param rate_limit - The delay between downloading episodes
			 * @return A season object
			 *
			 * Gets the season page, which is really just a series page, and creates a season object with all the episodes of the season
			 */
			static season get_season(const std::string& url, const cookie& session_cookie, bool download_captions, bool download_captions_only, uint32_t rate_limit);

			/**
			 *
			 * @param quality - The quality of the video
			 * @param base - The base directory to download to
			 *
			 * Downloads the series into the <b>base</b> directory with the format <i>\<base\>/\<series name\>/\<season name\>/\<episode\></i>
			 */
		   void download(const std::string& quality, const std::string& base = ".");

		   /**
			*
			* @param url - The link to the series page
			* @param cookies - The browser cookies
			*
			* Creates a series object and populates the needed variables
			*/
			series(const std::string& url, const dropout_dl::cookie& session_cookie, bool download_captions = false, bool download_captions_only = false, const uint32_t rate_limit = 2000) {
				this->url = url;
				this->download_captions = download_captions;
				this->download_captions_only = download_captions_only;
				this->page_data = get_generic_page(url);
				this->name = get_series_name(page_data);
				this->rate_limit = rate_limit;
				this->session_cookie = session_cookie;
				if (name == "ERROR") {
					std::cerr << "SERIES PARSE ERROR: Could not parse series name\n";
					exit(10);
				}

				this->series_directory = format_filename(name);

				this->seasons = this->get_seasons();
			}
	};

} // dropout_dl
