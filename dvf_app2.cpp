
#include "pbx.h"
#include <array>
#include <assert.h>
#include <fstream> // std::ifstream std::ofstream
#include <iomanip>
#include <iostream> // std::cout
#include <map>
#include <set>
#include <sstream> // std::sringstream
#include <string.h>
#include <string>
#include <vector>

//--------------------------------------------------------------------------------------------------
// https://medium.com/ml2b/linear-regression-implementation-in-c-acdfb621e56
// https://gist.github.com/ahmhashesh?page=3
//
// Returns true if linear fit was calculated. False otherwise.
// Algorithm adapted from:
// https://en.wikipedia.org/wiki/Simple_linear_regression#Fitting_the_regression_line
//
template <typename PairIterator>
bool GetLinearFit(PairIterator begin_it, PairIterator end_it, double *out_slope, double *out_yintercept)
{
  if (begin_it == end_it)
  {
    return false;
  }

  size_t n = 0;
  double x_avg = 0;
  double y_avg = 0;

  for (PairIterator it = begin_it; it != end_it; ++it)
  {
    x_avg += it->first;
    y_avg += it->second;
    n++;
  }

  x_avg /= (double)n;
  y_avg /= (double)n;

  double numerator = 0;
  double denominator = 0;

  for (PairIterator it = begin_it; it != end_it; ++it)
  {
    double x_variance = it->first - x_avg;
    double y_variance = it->second - y_avg;
    numerator += (x_variance * y_variance);
    denominator += (x_variance * x_variance);
  }

  double slope = numerator / denominator;
  double yintercept = y_avg - slope * x_avg;

  *out_slope = slope;
  *out_yintercept = yintercept;

  return true;
}

void test()
{
  std::vector<std::pair<int, int>> data;
  for (int i = 0; i < 10; ++i)
  {
    data.push_back(std::pair<int, int>(i + 1, 2 * i));
  }

  double slope = 0;
  double y_intercept = 0;
  GetLinearFit(data.begin(), data.end(), &slope, &y_intercept);

  std::cout << "slope: " << slope << "\n";
  std::cout << "y_intercept: " << y_intercept << "\n";
}

//--------------------------------------------------------------------------------------------------
// https://stackoverflow.com/questions/43482488/how-to-format-a-number-with-thousands-separator-in-c-c
struct separate_thousands : std::numpunct<char>
{
  char_type do_thousands_sep() const override
  {
    return ',';
  } // separate with commas
  string_type do_grouping() const override
  {
    return "\3";
  } // groups of 3 digit
};

void test_separate_thousands()
{
  int number = 123'456'789;
  std::cout << "default locale: " << number << '\n';
  auto thousands = std::make_unique<separate_thousands>();
  std::cout.imbue(std::locale(std::cout.getloc(), thousands.release()));
  std::cout << "locale with modified thousands: " << number << '\n';
}

std::string quickAddThousandSeparators(std::string value, char thousandSep = ',')
{
  int len = value.length();
  int dlen = 3;

  while (len > dlen)
  {
    value.insert(len - dlen, 1, thousandSep);
    dlen += 4;
    len += 1;
  }
  return value;
}

std::string addThousandSeparators(std::string value, char thousandSep = ',', char decimalSep = '.',
                                  char sourceDecimalSep = '.')
{
  int len = value.length();
  int negative = ((len && value[0] == '-') ? 1 : 0);
  int dpos = value.find_last_of(sourceDecimalSep);
  int dlen = 3 + (dpos == (int)std::string::npos ? 0 : (len - dpos));

  if (dpos != (int)std::string::npos && decimalSep != sourceDecimalSep)
  {
    value[dpos] = decimalSep;
  }

  while ((len - negative) > dlen)
  {
    value.insert(len - dlen, 1, thousandSep);
    dlen += 4;
    len += 1;
  }
  return value;
}

// https://codereview.stackexchange.com/questions/178200/format-an-integer-with-thousands-separator-recursive-implementation
template <class T> void RecursiveCommas(std::ostream &os, T n)
{
  T rest = n % 1000; //"last 3 digits"
  n /= 1000;         //"begining"

  if (n > 0)
  {
    RecursiveCommas(os, n); // printing "begining"

    // and last chunk
    os << ',' << std::setfill('0') << std::setw(3) << rest;
  }
  else
    os << rest; // first chunk of the number
}

// https://en.cppreference.com/w/cpp/locale/num_put

//--------------------------------------------------------------------------------------------------
static std::set<std::string> TYPE_LOCAL_36 = {
    {"Maison"},
    {"Appartement"},
    {"Dépendance"},
    {"Local industriel. commercial ou assimilé"},
};
static std::map<std::string, std::string> CODE_NATURE_CULTURE_40 = {
    {"AB", "terrains à bâtir"},
    {"AG", "terrains d’agrément"},
    {"B", "bois"},
    {"BF", "futaies feuillues"},
    {"BM", "futaies mixtes"},
    {"BO", "oseraies"},
    {"BP", "peupleraies"},
    {"BR", "futaies résineuses"},
    {"BS", "taillis sous futaie"},
    {"BT", "taillis simples"},
    {"CA", "carrières"},
    {"CH", "chemin de fer"},
    {"E", "eaux"},
    {"J", "jardins"},
    {"L", "landes"},
    {"LB", "landes boisées"},
    {"P", "prés"},
    {"PA", "pâtures"},
    {"PC", "pacages"},
    {"PE", "prés d’embouche"},
    {"PH", "herbages"},
    {"PP", "prés plantes"},
    {"S", "sols"},
    {"T", "terres"},
    {"TP", "terres plantées"},
    {"VE", "vergers"},
    {"VI", "vignes"},
};

//--------------------------------------------------------------------------------------------------
template <int REF> struct stridx
{
  std::vector<std::string> _strs = {};
  std::map<std::string, size_t> _stridx = {};
  stridx()
  {
    // on reserve la place [0] pour l'empty string
    _strs.push_back("");
  }
  stridx(const stridx &) = delete;
  stridx(stridx &&) = delete;
  stridx &operator=(const stridx &) = delete;
  stridx &operator=(stridx &&) = delete;
  size_t get_str_idx(const std::string &str_)
  {
    if (str_.empty())
    {
      if (REF > 0)
        throw REF;
      else
        // on reserve la place [0] pour l'empty string
        return 0;
    }
    auto found = _stridx.find(str_);
    if (found == _stridx.end())
    {
      _stridx.insert({str_, _strs.size()});
      _strs.push_back(str_);
      found = _stridx.find(str_);
    }
    return found->second;
  }
  const std::string &get_str(size_t idx_) const
  {
    return _strs[idx_];
  }
  const size_t size() const
  {
    return _strs.size();
  }
};

//--------------------------------------------------------------------------------------------------
struct evf_t
{
  static stridx<17> _map_to_17_address_city;
  static stridx<21> _map_to_21_section;
  static stridx<-36> _map_to_36_type_local;
  static stridx<-40> _map_to_40_code_nature_culture;

  char _8_t52[9];
  float _10_t31 = {};
  int _16_address_postal_code = {};
  size_t _17_address_city = {};
  size_t _21_section = {};
  size_t _36_type_local = {};
  float _38_surface = {};
  size_t _40_code_nature_culture = {};
  float _42_terrain = {};
  std::string _csv_line = {};

  const std::string &get_17_address_city() const
  {
    return _map_to_17_address_city.get_str(_17_address_city);
  }
  const std::string &get_21_section() const
  {
    return _map_to_21_section.get_str(_21_section);
  }
  const std::string &get_36_type_local() const
  {
    return _map_to_36_type_local.get_str(_36_type_local);
  }
  const std::string &get_40_code_nature_culture() const
  {
    return _map_to_40_code_nature_culture.get_str(_40_code_nature_culture);
  }
  std::ostringstream view(float _3842_ = -1) const
  {
    std::ostringstream price;
    RecursiveCommas(price, (int)_10_t31);

    std::ostringstream oss;
    oss << _16_address_postal_code;
    oss << " 52=" << _8_t52;
    // oss << " 31=" << std::right << std::setw(7) << std::fixed << std::setprecision(0) << _10_t31;
    // oss << " 31=" << std::right << std::setw(9) << price.str();
    oss << " 31=" << price.str();
    oss << " 38=" << std::fixed << std::setprecision(0) << _38_surface;
    oss << " 42=" << std::fixed << std::setprecision(0) << _42_terrain;
    if (_3842_ != -1)
      oss << " 3842=" << std::fixed << std::setprecision(1) << _3842_;

    oss << " " << get_17_address_city();
    oss << " 21=" << get_21_section();
    oss << " 36=" << get_36_type_local();
    oss << " 40=" << get_40_code_nature_culture();
    // oss << _11_address_no << " " << _13_address_type_de_voie << " " << _address_street << " " <<
    // << _add_state << " " << _21_section << " ";
    return oss;
  }
  const std::string &get_csv_line() const
  {
    return _csv_line;
  };
};

stridx<17> evf_t::_map_to_17_address_city = {};
stridx<21> evf_t::_map_to_21_section = {};
stridx<-36> evf_t::_map_to_36_type_local = {};
stridx<-40> evf_t::_map_to_40_code_nature_culture = {};

//--------------------------------------------------------------------------------------------------
struct evfs_t
{
  std::vector<evf_t> _evfs = {};
  size_t push_back_evf(const evf_t &data_)
  {
    size_t sz = _evfs.size();
    _evfs.push_back(data_);
    return sz;
  }
  size_t size() const
  {
    return _evfs.size();
  }
};

struct db_t
{
  evfs_t _evfs2;
  std::map<std::string, std::vector<size_t>> _keys;

  db_t() = default;
  db_t(const db_t &) = delete;
  db_t(db_t &&) = delete;
  db_t &operator=(const db_t &) = delete;
  db_t &operator=(db_t &&) = delete;

  void add_evf(const evf_t &evf_)
  {
    auto pos = _evfs2.push_back_evf(evf_);
    _keys[get_key(evf_)].push_back(pos); // l'ajout cette cle prend 30sec
  }
  size_t size() const
  {
    return _evfs2.size();
  }
  std::string get_key(const evf_t &evf_) const
  {
    std::ostringstream key; // key construction
    key << evf_._16_address_postal_code;
    key << evf_._8_t52;
    key << std::fixed << std::setprecision(0) << evf_._10_t31;
    return key.str();
  }
  const std::vector<size_t> *get_peers(const evf_t &evf_) const
  {
    auto found = _keys.find(get_key(evf_));
    return (found != _keys.end()) ? &(found->second) : nullptr;
  }
  const evf_t *get_evf(size_t pos_) const
  {
    return (pos_ < _evfs2._evfs.size()) ? &(_evfs2._evfs[pos_]) : nullptr;
  }
  std::map<std::string, float> get_surfaces(const evf_t &evf_) const
  {
    std::map<std::string, float> surfaces;
    auto found = _keys.find(get_key(evf_));
    if (found != _keys.end())
    {
      for (auto &pos : found->second)
      {
        const std::string &code36 = _evfs2._evfs[pos].get_36_type_local();
        const std::string &code40 = _evfs2._evfs[pos].get_40_code_nature_culture();
        if (code36 == "Maison" || code36 == "Appartement" || code40.data()[0] == 'S')
        {
          if (_evfs2._evfs[pos]._38_surface > 1.)
            surfaces[code40] += _evfs2._evfs[pos]._38_surface; // theoriquement c'est ca ...
          else
            surfaces[code40] += _evfs2._evfs[pos]._42_terrain; // ... mais des fois c'est ca
        }
        else
        {
          surfaces[code40] += _evfs2._evfs[pos]._42_terrain;
        }
      }
    }
    return surfaces;
  }
  float get_surface(const evf_t &evf_) const
  {
    std::map<std::string, float> surfaces = get_surfaces(evf_);
    float surface = {}, autre = {};
    for (auto &[key, val] : surfaces)
    {
      if (key.data()[0] == 'S')
        surface += val;
      else
        autre += val;
    }
    float terrain = {};
    if (surface > 1.)
    {
      if (autre < 5000.)
        terrain = surface;
      else
        terrain = surface + autre / 10'000.;
    }
    else
    {
      if (autre < 5000.)
        terrain = autre;
      else
        terrain = autre / 10'000.;
    }
    return terrain;
  }
  float get_rate(const evf_t &evf_) const
  {
    return evf_._10_t31 / (float)get_surface(evf_);
  }
  template <typename T, int N> static constexpr size_t get_size(T (&)[N])
  {
    return N;
  }
  template <int... POSTCODES>
  constexpr uint64_t preview(const char *title_, std::initializer_list<std::string> section_21_,
                             std::initializer_list<std::string> type_local_36_,
                             std::initializer_list<std::string> code_nature_culture_40_) const
  {
    std::cout << "=========================================================================" << std::endl;
    std::cout << "=== " << title_ << std::endl;
    std::cout << "=========================================================================" << std::endl;
    uint64_t T0 = gettime();
    constexpr int postcodes[] = {POSTCODES...};
    // constexpr size_t NB_POSTCODES = get_size(postcodes);
    std::set<std::string> sections = section_21_;
    std::set<std::string> local_types = type_local_36_;
    std::set<std::string> code_nature_cultures = code_nature_culture_40_;

    std::map<float, std::vector<std::string>> maposs;
    int count1 = {};
    for (const auto &[key, vect] : _keys)
    {
      int count2 = {};
      for (const auto &pos : vect)
      {
        const auto &it = _evfs2._evfs[pos];

        const int *found = std::find(std::begin(postcodes), std::end(postcodes), it._16_address_postal_code);
        bool b = (found != std::end(postcodes));
        b = b && (sections.empty() || (sections.find(it.get_21_section()) != sections.end()));
        b = b && (local_types.empty() || (local_types.find(it.get_36_type_local()) != local_types.end()));
        b = b && (code_nature_cultures.empty() ||
                  (code_nature_cultures.find(it.get_40_code_nature_culture()) != code_nature_cultures.end()));
        if (b)
        {
          float surface = get_surface(it);
          float rate = get_rate(it);
          std::ostringstream oss;
          oss << std::setw(5) << ++count1 << "." << std::left << std::setw(2) << ++count2;
          oss << "/" << vect.size();
          // oss << " " << std::fixed << std::setprecision(1) << rate;
          oss << " " << it.view(surface).str();
          maposs[rate].push_back(oss.str());

          // debug function display:
          if (vect.size() > 1)
          {
            for (const auto &pos : vect)
            {
              const auto &it = _evfs2._evfs[pos];
              maposs[rate].push_back(it.get_csv_line());
            }
          }
        }
      }
    }
    for (const auto &[rate, vect] : maposs)
    {
      for (const auto &str : vect)
      {
        std::cout << std::setw(7) << std::fixed << std::setprecision(1) << rate << " ";
        std::cout << str << std::endl;
      }
    }
    return gettime() - T0;
  }
#if false
  template <int... POSTCODES>
  constexpr uint64_t preview(const char *title_, std::initializer_list<std::string> section_21_,
                             std::initializer_list<std::string> type_local_36_,
                             std::initializer_list<std::string> code_nature_culture_40_) const
  {
    std::cout << "=========================================================================" << std::endl;
    std::cout << "=== " << title_ << std::endl;
    std::cout << "=========================================================================" << std::endl;
    uint64_t T0 = gettime();
    constexpr int postcodes[] = {POSTCODES...};
    // constexpr size_t NB_POSTCODES = get_size(postcodes);
    std::set<std::string> sections = section_21_;
    std::set<std::string> local_types = type_local_36_;
    std::set<std::string> code_nature_cultures = code_nature_culture_40_;

    int count = {};
    for (const auto &it : _evfs2._evfs)
    {
      const int *found = std::find(std::begin(postcodes), std::end(postcodes), it._16_address_postal_code);
      bool b = (found != std::end(postcodes));
      b = b && (sections.empty() || (sections.find(it.get_21_section()) != sections.end()));
      b = b && (local_types.empty() || (local_types.find(it.get_36_type_local()) != local_types.end()));
      b = b && (code_nature_cultures.empty() ||
                (code_nature_cultures.find(it.get_40_code_nature_culture()) != code_nature_cultures.end()));
      if (b)
      {
        std::cout << std::setw(5) << ++count << " " << it.view().str() << std::endl;
        const std::vector<size_t> *p = get_peers(it);
        if (p != nullptr && p->size() > 1)
        {
          const std::string& ref = it.get_csv_line();
          for (const auto &peer : *p)
          {
            const evf_t *pp = get_evf(peer);
            const std::string& a = pp->get_csv_line();
            if( ref != a )
              std::cout << "      " << pp->view().str() << std::endl;
          }
        }
      }
    }
    return gettime() - T0;
  }
#endif
};

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[], char **argenv)
{
  if (argc == 2)
  {
    // argv[1] return 0;
  }

  db_t DB = {};
  int _8_empty_date = {};
  int _10_empty_price = {};
  int _17_empty_address_city = {};
  int _21_empty_section = {};
  int _36_40_empty_code = {};

  // clang-format off
  uint64_t T0 = gettime();
  pbx::dirent_flat_parsing("./valeursfoncieres",
    [&](int dirfd_, const std::string &path_, struct dirent *epdf_, struct stat &stat_) -> void
    {
      std::cout << "path = " << path_ << AT << std::endl;
      auto str = std::ifstream(path_, std::ios::in);
      {
        auto tokens = pbx::parse_csv_header(str);
      }

     auto push_evf_t = [&](pbx::Tokens &tokens_, const std::string& line_)
      {
// |||||||000001|16/12/2015|Vente|210000,00||||B086|LE VILLAGE|34600|HEREPIAN|34|119||A|3861|8|||||||||||0||||||||
// |||||||000001|22/12/2015|Vente|153965,10|||ZAC|A002|DE BASTIT|34500|BEZIERS|34|32||HS|901|2|||||||||||0||||||||

        evf_t data {
                                                //  0 Code service CH
                                                //  1 Référence document
                                                //  2 Article CGI 1
                                                //  3 Article CGI 2
                                                //  4 Article CGI 3
                                                //  5 Article CGI 4
                                                //  6 Article CGI 5
                                                //  7 N°de disposition - Seules les dispositions concernant les mutations à titre onéreux sont restituées dans le fichier.
         // ._8_t52 = tokens_.at(8),            //  8 Date de mutation
                                                //  9 Nature de la mutation - Vente, vente en l’état futur d’achèvement, vente de terrain à bâtir, adjudication, expropriation ou échange
            ._10_t31 = (float)atof( tokens_.at(10).data()), // 10 Valeur foncière - Il s’agit du montant ou de l’évaluation déclaré dans le cadre d’une mutation à titre onéreux. La valeur foncière est le prix net vendeur. La TVA est incluse. Ce prix n’inclut pas, en revanche, les frais de notaires.
         // ._11_address_no = tokens_.at(11),   // 11 N° de voie
         // ._12_address_BTQ = tokens_.at(12),  // 12 B/T/Q - Indice de répétition
         // ._13_address_type_de_voie = tokens_.at(13), // 13 Type de voie - Exemple : Rue, avenue, etc.
                                                // 14 Code voie
         // ._address_street = tokens_.at(15),  // 15 Voie - Libellé de la voie
            ._16_address_postal_code = std::atoi( tokens_.at(16).data()), // 16 Code postal
         // ._17_address_city = _map_to_17_address_city.get_str_idx( tokens_.at(17) ), // 17 Commune - Libellé de la commune
         // ._18_add_state = std::atoi( tokens_.at(18).data()), // 18 Code département
                                                // 19 Code commune
                                                // 20 Préfixe de section
         //._21_section = _map_to_21_section.get_str_idx( tokens_.at(21)), // 21 Section
                                                // 22 N° de plan
                                                // 23 N° de volume
                                                // 24 1er lot
                                                // 25 Surface Carrez du 1er lot
                                                // 26 2e lot
                                                // 27 Surface Carrez du 2e lot
                                                // 28 3e lot
                                                // 29 Surface Carrez du 3e lot
                                                // 30 4e lot
                                                // 31 Surface Carrez du 4e lot
                                                // 32 5e lot
                                                // 33 Surface Carrez du 5e lot
                                                // 34 Nombre de lots
            // 35 Code type  local: 1 : maison ; 2 : appartement ; 3 : dépendance (isolée) ; 4 : local industriel et commercial ou assimilés
            //._36_type_local = _map_to_36_type_local.get_str_idx( tokens_.at(36)), // 36 Type local : Maison Appartement Dépendance
            // 37 Identifiant local 1 : maison ; 2 : appartement ; 3 : dépendance (isolée) ; 4 : local industriel et commercial ou assimilés
            ._38_surface = (float)atof(tokens_.at(38).data()), // 38 Surface réelle
         // ._39_nopieces = tokens_.at(39),     // 39 Nombre de pièces principales
         // ._40_code_nature_culture = tokens_.at(40), // 40 Code nature culture
                                                // 41 Nature culture spéciale
            ._42_terrain = (float)atof(tokens_.at(42).data()), // 42 Surface terrain - Contenance du terrain
          };
          // clang-format on

          try
          {
            // 20/07/2017
            if (!tokens_.at(8).empty())
            {
              const char *p = tokens_.at(8).data(); //  8 Date de mutation
              data._8_t52[0] = p[6];
              data._8_t52[1] = p[7];
              data._8_t52[2] = p[8];
              data._8_t52[3] = p[9];
              data._8_t52[4] = p[3];
              data._8_t52[5] = p[4];
              data._8_t52[6] = p[0];
              data._8_t52[7] = p[1];
              data._8_t52[8] = '\0';
            }
            else
            {
              throw 8;
            }

            if (data._10_t31 == 0)
            {
              throw 10;
            }
            data._17_address_city = evf_t::_map_to_17_address_city.get_str_idx(tokens_.at(17));
            data._21_section = evf_t::_map_to_21_section.get_str_idx(tokens_.at(21));
            data._36_type_local = evf_t::_map_to_36_type_local.get_str_idx(tokens_.at(36));
            data._40_code_nature_culture = evf_t::_map_to_40_code_nature_culture.get_str_idx(tokens_.at(40));
            if (data._36_type_local == 0 && data._40_code_nature_culture == 0)
              throw 3640;
            data._csv_line = line_;
            DB.add_evf(data);
          }
          catch (int ref)
          {
            switch (ref)
            {
            case 8:
              ++_8_empty_date;
              break;
            case 10:
              ++_10_empty_price;
              break;
            case 17:
              ++_17_empty_address_city;
              break;
            case 21:
              ++_21_empty_section;
              break;
            case 3640:
              ++_36_40_empty_code;
              // std::cout << line_ << std::endl;
              break;
            default:
              break;
            }
          }
        };
        pbx::parse_csv_file(push_evf_t, str, '|');
      });

  uint64_t T1 = gettime();

  std::cout << "time       = " << (T1 - T0) / 1'000'000'000. << " sec" << AT << std::endl;
  std::cout << "_map_to_17_address_city.size()        = " << evf_t::_map_to_17_address_city.size() << std::endl;
  std::cout << "_map_to_21_section.size()             = " << evf_t::_map_to_21_section.size() << std::endl;
  std::cout << "_map_to_36_type_local.size()          = " << evf_t::_map_to_36_type_local.size() << std::endl;
  std::cout << "_map_to_40_code_nature_culture.size() = " << evf_t::_map_to_40_code_nature_culture.size() << std::endl;
  std::cout << " _8_empty_date          = " << _8_empty_date << std::endl;
  std::cout << " _10_empty_price        = " << _10_empty_price << std::endl;
  std::cout << " _17_empty_address_city = " << _17_empty_address_city << std::endl;
  std::cout << " _21_empty_section      = " << _21_empty_section << std::endl;
  std::cout << " _36_40_empty_code      = " << _36_40_empty_code << std::endl;
  int total = _8_empty_date + _10_empty_price + _17_empty_address_city + _21_empty_section + _36_40_empty_code;
  std::cout << "total        = " << DB.size() << std::endl;
  std::cout << "total missed = " << total << " (" << total / float(DB.size()) << ")" << std::endl;

  DB.preview<17000>("*** the title ***", {}, // std::initializer_list<std::string> section_21_

                    //{}, // std::initializer_list<std::string> type_local_36_
                    {"Maison"}, // std::initializer_list<std::string> type_local_36_

                    //{"P", "T"}); //std::initializer_list<std::string> code_nature_culture_40_
                    {}); // std::initializer_list<std::string> code_nature_culture_40_
  return 0;
}

// rsync -avzh ~/tmp
// rsync -av oulavie/ /media/fullname/8B12-298C/oulavie --exclude='.git/'
// https://sethrobertson.github.io/GitFixUm/fixup.html
// ldd ./a.out
// readelf -a ./a.out                 -> NEEDED
// objdump -p ./a.out                 -> rpath
// valgrind --tool=memcheck program_name --leak-check=yes ./a.out
// LD_DEBUG=libs ldd <executable>
// clang-format-9 -i dvf_app2.cpp
// Demande de Valeur Foncière
// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 -I../libx -O3 dvf_app2.cpp
// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
