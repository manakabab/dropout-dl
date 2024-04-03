//
// Created by moss on 9/29/22.
//

#include "series.h"

namespace dropout_dl {

	std::string series::get_series_name(const std::string& html_data) {
		std::string collection_title("collection-title");
		std::string close_tag(">");
		std::string close_a("</h1>");

		for (int i = 0; i < html_data.size(); i++) {
			if (substr_is(html_data, i, collection_title)) {
				for (int j = i + collection_title.size(); j < html_data.size(); j++) {
					if (html_data[j] == '\n' || html_data[j] == ' ' || html_data[j] == '\t') continue;
					if (substr_is(html_data, j, close_tag)) {
						for (int l = 0; l < html_data.size() - j; l++) {
							if (substr_is(html_data, j + l, close_a)) {
								return format_name_string(html_data.substr(j + 1, l - 1));
							}
						}
					}
				}
			}
		}
		return "ERROR";
	}

	std::vector<season> series::get_seasons() {
		std::vector<season> out;

		std::string search_class("js-switch-season");
		std::string open_select("<select");
		std::string close_tag(">");
		std::string close_select("</select>");

		std::string open_option("<option");
		std::string close_option("</option>");
		std::string value("value=");

		bool seasons_dropdown = false;
		std::string season_url;
		std::string season_name;
		for (int i = 0; i < this->page_data.size(); i++) {
			if (substr_is(this->page_data, i, open_select)) {
				for (int j = i; j < this->page_data.size(); j++) {
					if (substr_is(this->page_data, j, search_class)) {
						i = j;
						seasons_dropdown = true;
						break;
					}
					else if (substr_is(this->page_data, j, close_tag)) {
						break;
					}
				}
			}
			if (seasons_dropdown) {
				if (substr_is(this->page_data, i, value)) {
					i += value.size() + 1;
					for (int j = 0; j + i < this->page_data.size(); j++) {
						if (this->page_data[i + j] == '"') {
							season_url = this->page_data.substr(i, j);
							i += j;
							break;
						}
					}
				}
				else if (!season_url.empty() && substr_is(this->page_data, i, close_tag)) {
					i += close_tag.size() + 1;
					for (int j = 0; i + j < this->page_data.size(); j++) {
						if (this->page_data[i + j] == '\n') {
							season_name = this->page_data.substr(i, j);

							// Remove leading and trailing whitespace
							bool leading_whitespace = true;
							int name_start;
							int name_end;
							for (int k = 0; k < season_name.size(); k++) {
								if (season_name[k] != ' ' && season_name[k] != '\t' && season_name[k] != '\n') {
									name_start = k;
									break;
								}
							}
							for (int k = season_name.size() - 1; k > 0; k--) {
								if (season_name[k] != ' ' && season_name[k] != '\t' && season_name[k] != '\n') {
									name_end = k;
									break;
								}
							}
							season_name = season_name.substr(name_start, season_name.size() - name_start - name_end);

							out.emplace_back(season_url, season_name, this->session_cookie, this->name, this->download_captions, this->download_captions_only, this->rate_limit);

							std::cout << out.back().name << ": " << out.back().url << '\n';

							season_url.clear();
							season_name.clear();

							i = i + j;

							break;
						}
					}
				}

				if (substr_is(this->page_data, i, close_select)) {
					break;
				}
			}
		}

		return out;
	}


	season series::get_season(const std::string &url, const cookie& session_cookie, bool download_captions, bool download_captions_only = false, uint32_t rate_limit = 2000) {
		std::string html_data = get_generic_page(url);

		std::string search_class("js-switch-season");
		std::string open_select("<select");
		std::string close_tag(">");
		std::string close_select("</select>");

		std::string open_option("<option");
		std::string close_option("</option>");
		std::string value("value=");

		bool seasons_dropdown = false;
		bool selected = false;
		std::string season_url;
		std::string season_name;
		for (int i = 0; i < html_data.size(); i++) {
			if (substr_is(html_data, i, open_select)) {
				for (int j = i; j < html_data.size(); j++) {
					if (substr_is(html_data, j, search_class)) {
						i = j;
						seasons_dropdown = true;
						break;
					}
					else if (substr_is(html_data, j, close_tag)) {
						break;
					}
				}
			}
			if (seasons_dropdown) {
				if (substr_is(html_data, i, value)) {
					i += value.size() + 1;
					for (int j = 0; j + i < html_data.size(); j++) {
						if (html_data[i + j] == '"') {
							season_url = html_data.substr(i, j);
							i += j;
							break;
						}
					}
				}
				else if (!season_url.empty() && substr_is(html_data, i, "selected")) {
					selected = true;
				}
				else if (!season_url.empty() && substr_is(html_data, i, close_tag)) {
					i += close_tag.size() + 1;
					for (int j = 0; i + j < html_data.size(); j++) {
						if (html_data[i + j] == '\n') {
							if (selected) {
								season_name = html_data.substr(i, j);

								// Remove leading and trailing whitespace
								bool leading_whitespace = true;
								int name_start;
								int name_end;
								for (int k = 0; k < season_name.size(); k++) {
									if (season_name[k] != ' ' && season_name[k] != '\t' && season_name[k] != '\n') {
										name_start = k;
										break;
									}
								}
								for (int k = season_name.size() - 1; k > 0; k--) {
									if (season_name[k] != ' ' && season_name[k] != '\t' && season_name[k] != '\n') {
										name_end = k;
										break;
									}
								}
								season_name = season_name.substr(name_start,
																 season_name.size() - name_start - name_end);

								return {season_url, season_name, session_cookie, get_series_name(html_data), download_captions, download_captions_only, rate_limit};
							}

							season_url.clear();
							season_name.clear();

							i = i + j;

							break;
						}
					}
				}

				if (substr_is(html_data, i, close_select)) {
					break;
				}
			}
		}

		std::cerr << "SEASON PARSE ERROR: No selected season found\n";
		exit(9);
	}

	void series::download(const std::string &quality, const std::string& base) {
		if (!std::filesystem::is_directory(base + "/" + series_directory)) {
			std::filesystem::create_directories(base + "/" + series_directory);
			std::cout << "Creating series directory" << '\n';
		}

		for (auto& season : seasons) {
			season.download(quality, base + "/" + series_directory);
		}
	}

} // dropout_dl
