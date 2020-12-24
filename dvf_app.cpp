
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

// clang-format off
//
// https://atlas.cosmosia.com/map/
// https://www.data.gouv.fr/fr/datasets/base-officielle-des-codes-postaux/
//
// https://www.data.gouv.fr/fr/datasets/5c4ae55a634f4117716d5656/
// |||||||000002|20/11/2015|Vente|295000,00|16||RUE|5790|DU MOULIN ROUGE|17000|LA ROCHELLE|17|300||CR|459||||||||||||0|1|Maison||94|4|S||262
// |||||||000001|27/08/2019|Vente|705000,00|20||RUE|5790|DU MOULIN ROUGE|17000|LA ROCHELLE|17|300||CR|131||||||||||||0|1|Maison||128|7|S||442
// https://app.dvf.etalab.gouv.fr/        Demande de Valeur Foncière
// 705 000 € / Vente; 26/08/2019; 20 RUE DU MOULIN ROUGE; 128 m²; Maison / 7 p; 442 m²; sols; 5508 € / m²
// https://github.com/etalab/DVF-app
// clang-format on
//
struct evf_t
{
  char _8_t52[9];
  float _10_t31;
  // std::string _no802 = "4";
  // 803=34,523=Address City
  // 803=35,523=Address State/Province
  // 803=36,523=Address Postal Code
  // 803=37,523=Address Street
  int _16_address_postal_code;
  std::string _17_address_city;
  // int _add_state;
  // std::string _11_address_no, _12_address_BTQ, _13_address_type_de_voie, _address_street, _16_address_postal_code,
  // _17_address_city, _add_state;
  std::string _21_section;
  std::string _36_type_local;
  float _38_surface;
  std::string _40_code_nature_culture;
  float _42_terrain;
  // std::string _36_type_local, _38_surface, _39_nopieces, _42_terrain;

  std::ostringstream view2(float rate_)
  {
    std::ostringstream oss;
    oss << std::setw(8) << std::fixed << std::setprecision(1) << rate_ << "; ";
    oss << _21_section << "; ";
    oss << _8_t52 << "; ";
    oss << std::setw(7) << std::fixed << std::setprecision(0) << _10_t31 << "; ";
    oss << std::setfill(' ') << std::setw(12) << _36_type_local << "; ";
    oss << std::setfill(' ') << std::setw(2) << _40_code_nature_culture << "; ";
    oss << std::setw(7) << std::fixed << _38_surface << "; ";
    oss << std::setw(7) << std::fixed << _42_terrain << "; ";
    // oss << _11_address_no << " " << _13_address_type_de_voie << " " << _address_street << " " <<
    // _16_address_postal_code << " " << _17_address_city << " "
    // << _add_state << " ";
    return oss;
  }
  // float getRate() const
  // {
  //   return (_38_surface > 1.) ? (_10_t31 / _38_surface) : (_10_t31 / (_42_terrain/10'000.));
  // }
  std::ostringstream view()
  {
    std::ostringstream oss;
    oss << _8_t52 << " " << _10_t31 << " ";
    oss << _36_type_local << " " << _38_surface << " " << _42_terrain << " " << _10_t31 / _38_surface << " ";
    // oss << _11_address_no << " " << _13_address_type_de_voie << " " << _address_street << " " <<
    // _16_address_postal_code << " " << _17_address_city << " "
    // << _add_state << " " << _21_section << " ";
    return oss;
  }
};

//--------------------------------------------------------------------------------------------------
template <size_t N, size_t MIN, size_t MAX> struct stat_t
{
  float STEP = (MAX - MIN) / float(N);
  size_t bucket[N];
  std::vector<float> _vals = {};

  stat_t()
  {
    for (size_t i(0); i < N; ++i)
      bucket[i] = 0;
  }
  void add2stat(float val_)
  {
    if (val_ > MIN && val_ < MAX)
    {
      float r = (val_ - MIN) / STEP;
      size_t buck = static_cast<size_t>(r);
      ++bucket[buck];
      _vals.push_back(val_);
    }
  }
  float val_median()
  {
    if (_vals.size() == 0)
      return 0.0;
    std::sort(_vals.begin(), _vals.end(), [](const auto &lhs_, const auto &rhs_) -> bool { return lhs_ < rhs_; });
    return _vals[_vals.size() / 2];
  }
  size_t get_total() const
  {
    size_t total = 0;
    for (size_t i(0); i < N; ++i)
      total += bucket[i];
    return total;
  }
  std::ostringstream view_header()
  {
    std::ostringstream oss;
    for (size_t i(0); i < N; ++i)
    {
      oss << std::setw(7) << std::fixed << std::setprecision(0) << MIN + (i)*STEP << " ";
    }
    return oss;
  }
  std::ostringstream view()
  {
    size_t total = 0;
    for (size_t i(0); i < N; ++i)
      total += bucket[i];

    std::ostringstream oss;
    for (size_t i(0); i < N; ++i)
    {
      oss << MIN + (i)*STEP << " " << MIN + (i + 1) * STEP << " ";
      oss << std::setw(4) << std::fixed << bucket[i] << " ";
      // oss << 100.*float(bucket[i])/float(total) << std::endl;
      oss << std::setw(4) << std::fixed << std::setprecision(1) << 100. * float(bucket[i]) / float(total) << std::endl;
    }
    float med = val_median();
    oss << "median = " << med << std::endl;
    return oss;
  }
  std::ostringstream view2()
  {
    std::ostringstream oss;
    size_t total = get_total();
    for (size_t i(0); i < N; ++i)
    {
      // oss << MIN + (i)*STEP << " " << MIN + (i + 1) * STEP << " ";
      oss << std::setw(3) << std::fixed << bucket[i] << "/";
      oss << std::setw(2) << std::fixed << std::setprecision(0) << 100. * float(bucket[i]) / float(total) << "% ";
    }
    // float med = val_median();
    // oss << med << "€";
    return oss;
  }
};

//--------------------------------------------------------------------------------------------------
struct evfs_t
{
  std::vector<evf_t> _evfs = {};
  size_t push_back_data(const evf_t &data_)
  {
    size_t sz = _evfs.size();
    _evfs.push_back(data_);
    return sz;
  }
};

template <int POSTCODE_MAX> struct db_t
{
  evfs_t _evfs2[POSTCODE_MAX];
  std::map<std::string, std::vector<size_t>> _keys[POSTCODE_MAX]; // pas optimal :-(
  int total = {};
  int empty = {};

  void add_data(const evf_t &data_)
  {
    int postcode = data_._16_address_postal_code;
    if (postcode > 0 && postcode < POSTCODE_MAX)
    {
      if ((data_._36_type_local.empty() && data_._40_code_nature_culture.empty()) ||
          (data_._38_surface == 0 && data_._42_terrain == 0) || (data_._10_t31 == 0))
      {
        ++empty;
      }
      else
      {
        ++total;
        auto pos = _evfs2[postcode].push_back_data(data_);
        std::ostringstream key; // key construction
        key << data_._8_t52;
        key << std::fixed << std::setprecision(0) << data_._10_t31;
        _keys[postcode][key.str()].push_back(pos);
      }
    }
    else
    {
      std::cout << "POSTCODE OUT OF RANGE:" << postcode << AT << std::endl;
    }
  }

  constexpr float get_rate(const evf_t &evf_, std::ostringstream &debug_) const
  {
    float rate = {};
    int postcode = evf_._16_address_postal_code;
    std::ostringstream keystr; // keystr construction
    keystr << evf_._8_t52;
    keystr << std::fixed << std::setprecision(0) << evf_._10_t31;
    auto found = _keys[postcode].find(keystr.str());
    if (found != _keys[postcode].end())
    {
      std::map<std::string, float> surfaces;
      debug_ << "> ";
      // debug_ << postcode << " ";
      for (auto &pos : found->second)
      {
        const std::string &code40 = _evfs2[postcode]._evfs[pos]._40_code_nature_culture;

        // > 0/9134(ZK,T), 0/8276(ZN,VI), 0/4517(ZN,T), 0/7486(ZN,VI),
        // > 0/880(ZN,T), 0/480(ZN,S), 0/636(ZN,S), 0/3313(ZN,T),
        // > 90/500(ZN,S), 90/1694(ZN,T),
        // debug_ << std::fixed << std::setprecision(0) << _evfs2[postcode]._evfs[pos]._38_surface << "/" <<
        // _evfs2[postcode]._evfs[pos]._42_terrain; debug_ << "(" << _evfs2[postcode]._evfs[pos]._21_section << "," <<
        // code40 << "), ";
        if (code40.data()[0] == 'S')
        {
          if (_evfs2[postcode]._evfs[pos]._38_surface > 1.)
            surfaces[code40] += _evfs2[postcode]._evfs[pos]._38_surface; // theoriquement c'est ca ...
          else
            surfaces[code40] += _evfs2[postcode]._evfs[pos]._42_terrain; // ... mais des fois c'est ca
        }
        else
        {
          surfaces[code40] += _evfs2[postcode]._evfs[pos]._42_terrain;
        }
      }

      float surface = {}, autre = {};
      for (auto &[key, val] : surfaces)
      {
        debug_ << " " << key << "=" << std::fixed << std::setprecision(0) << val << " ";
        if (key.data()[0] == 'S')
          surface += val;
        else
          autre += val;
      }
      debug_ << " surface=" << std::fixed << std::setprecision(0) << surface;
      debug_ << " autre=" << std::fixed << std::setprecision(0) << autre;
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
      debug_ << " terrain=" << std::fixed << std::setprecision(3) << terrain;
      rate = evf_._10_t31 / terrain;
      // debug_ << " terrain=" << std::fixed << std::setprecision(1) << terrain;
      // debug_ << " px=" << std::fixed << std::setprecision(0) << evf_._10_t31;
      // debug_ << " rate=" << std::setprecision(1) << rate;
      // std::cout << debug_.str() << std::endl;
    }
    else
    {
      exit(0);
      if (evf_._40_code_nature_culture == "S")
        rate = evf_._10_t31 / evf_._38_surface;
      else
        rate = evf_._10_t31 / (evf_._42_terrain / 10'000.);
    }
    return rate;
  }

  template <int POSTCODE>
  constexpr uint64_t preview(const char *title_, std::initializer_list<std::string> section_,
                             std::initializer_list<std::string> type_local_,
                             std::initializer_list<std::string> code_nature_culture_) const
  {
    uint64_t T0 = gettime();
    std::set<std::string> sections = section_;
    std::set<std::string> local_types = type_local_;
    std::set<std::string> code_nature_cultures = code_nature_culture_;

    std::cout << "====================================================================================================="
              << AT << std::endl;
    std::cout << "=== " << title_;
    if (!local_types.empty())
    {
      std::cout << " -";
      for (auto &it : local_types)
        std::cout << " " << it;
    }
    if (!code_nature_cultures.empty())
    {
      std::cout << " -";
      for (auto &it : code_nature_cultures)
        std::cout << " " << it;
    }
    std::cout << " ===" << std::endl;
    std::cout << "====================================================================================================="
              << AT << std::endl;

    const evfs_t &evfs = _evfs2[POSTCODE];
    for (auto it : evfs._evfs)
    {
      bool b = true;
      b = b && (sections.empty() || (sections.find(it._21_section) != sections.end()));
      b = b && (local_types.empty() || (local_types.find(it._36_type_local) != local_types.end()));
      b = b && (code_nature_cultures.empty() ||
                (code_nature_cultures.find(it._40_code_nature_culture) != code_nature_cultures.end()));
      if (b)
      {
        std::ostringstream debug;
        float rate = this->get_rate(it, debug);
        // std::cout << it.view2(rate).str() << std::endl;
        std::cout << it.view2(rate).str() << " " << debug.str() << std::endl;
      }
    }
    return gettime() - T0;
  }

  template <typename T, int N> static constexpr size_t get_size(T (&)[N])
  {
    return N;
  }

  template <size_t N, size_t MIN, size_t MAX, int... POSTCODES>
  constexpr uint64_t process(const char *title_, std::initializer_list<std::string> type_local_,
                             std::initializer_list<std::string> code_nature_culture_) const
  {
    uint64_t T0 = gettime();
    constexpr int _postcodes[] = {POSTCODES...};
    constexpr size_t NB_POSTCODES = get_size(_postcodes);
    std::map<std::string, stat_t<N, MIN, MAX>> _stats[NB_POSTCODES] = {};
    std::set<std::string> local_types = type_local_;
    std::set<std::string> code_nature_cultures = code_nature_culture_;

    for (size_t i(0); i < NB_POSTCODES; ++i)
    {
      // std::cout << i << " " << postcode << AT << std::endl;
      int postcode = _postcodes[i];
      const evfs_t &evfs = _evfs2[postcode];
      for (auto it : evfs._evfs)
      {
        bool b = true;
        b = b && (local_types.empty() || (local_types.find(it._36_type_local) != local_types.end()));
        b = b && (code_nature_cultures.empty() ||
                  (code_nature_cultures.find(it._40_code_nature_culture) != code_nature_cultures.end()));
        if (b)
        {
          std::ostringstream debug;
          float rate = this->get_rate(it, debug);
          _stats[i][it._21_section].add2stat(rate);
        }
      }
    }

    struct anal_t
    {
      int _postcode;
      std::string _region;
      stat_t<N, MIN, MAX> *_pstat;
      float _median;
    };

    std::vector<anal_t> anals;
    for (size_t i(0); i < NB_POSTCODES; ++i)
    {
      for (auto &[key, val] : _stats[i])
      {
        if (val.get_total() > 0)
        {
          anal_t anal{
              ._postcode = _postcodes[i],
              ._region = key,
              ._pstat = &val,
              ._median = val.val_median(),
          };
          anals.push_back(anal);
        }
      }
    }

    std::sort(anals.begin(), anals.end(),
              [](const auto &lhs_, const auto &rhs_) -> bool { return lhs_._median < rhs_._median; });

    std::cout << "====================================================================================================="
              << AT << std::endl;
    std::cout << "=== " << title_;
    if (!local_types.empty())
    {
      std::cout << " -";
      for (auto &it : local_types)
        std::cout << " " << it;
    }
    if (!code_nature_cultures.empty())
    {
      std::cout << " -";
      for (auto &it : code_nature_cultures)
        std::cout << " " << it;
    }
    std::cout << " ===" << std::endl;
    std::cout << "====================================================================================================="
              << AT << std::endl;
    size_t idx = 0;
    for (auto &it : anals)
    {
      if (idx == 0)
        std::cout << "                      " << it._pstat->view_header().str() << std::endl;

      std::cout << std::setw(3) << std::fixed << std::setprecision(0) << ++idx << " ";
      std::cout << std::right << std::setw(5) << std::fixed << std::setprecision(0) << it._median << "€ ";
      std::cout << std::right << std::setw(4) << std::fixed << std::setprecision(0) << it._pstat->get_total() << " ";
      std::cout << std::left << std::setw(6) << it._postcode;
      std::cout << std::left << std::setw(2) << it._region << " ";
      std::cout << it._pstat->view2().str() << std::endl;
    }
    return gettime() - T0;
  }
};

//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[], char **argenv)
{
  if (argc == 2)
  {
    // argv[1] return 0;
  }

  db_t<99999> DB;

  int missing_postal_code = {}, missing_code = {}, missing_surface = {}, missing_price = {};
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
// Header:
// 0:  Code service CH|Reference document|1 Articles CGI|2 Articles CGI|3 Articles CGI|4 Articles CGI|5 Articles CGI|No disposition|Date mutation|Nature mutation|
// 10: Valeur fonciere|No voie|B/T/Q|Type de voie|Code voie|Voie|Code postal|Commune|Code departement|Code commune|Prefixe de section|Section|No plan|No Volume|
// 24: 1er lot|Surface Carrez du 1er lot|2eme lot|Surface Carrez du 2eme lot|3eme lot|Surface Carrez du 3eme lot|
// 30: 4eme lot|Surface Carrez du 4eme lot|5eme lot|Surface Carrez du 5eme lot|
// 34: Nombre de lots|Code type local|Type local|Identifiant local|Surface reelle bati|
// 39: Nombre pieces principales|Nature culture|Nature culture speciale|Surface terrain
//                   8               10      11   13         15            16      17       18 19   21  22                 36     38      42
// |||||||000002|20/11/2015|Vente|295000,00|16||RUE|5790|DU MOULIN ROUGE|17000|LA ROCHELLE|17|300||CR|459||||||||||||0|1|Maison||94|4|S||262
// |||||||000001|27/08/2019|Vente|705000,00|20||RUE|5790|DU MOULIN ROUGE|17000|LA ROCHELLE|17|300||CR|131||||||||||||0|1|Maison||128|7|S||442
// 705 000 € / Vente; 26/08/2019; 20 RUE DU MOULIN ROUGE; 128 m²; Maison / 7 p; 442 m²; sols; 5508 € / m²

// |||||||000001|22/12/2016|Vente|58800,00||||B094|L ENCLOUSE|17500|OZILLAC|17|270||ZN|17||||||||||||0||||||T||2380
// |||||||000001|22/12/2016|Vente|58800,00||||B094|L ENCLOUSE|17500|OZILLAC|17|270||ZN|15||||||||||||0||||||T||62520
// |||||||000001|22/12/2016|Vente|58800,00||||B094|L ENCLOUSE|17500|OZILLAC|17|270||ZN|14||||||||||||0||||||T||29500
// |||||||000001|22/12/2016|Vente|58800,00||||B094|L ENCLOUSE|17500|OZILLAC|17|270||ZN|12||||||||||||0||||||T||41050
// |||||||000001|22/12/2016|Vente|58800,00||||B008|BOIS DE L HERVOIR|17500|OZILLAC|17|270||ZM|101||||||||||||0||||||BT||5205
// |||||||000001|22/12/2016|Vente|58800,00||||B066|CHEZ ESTEFFE|17500|OZILLAC|17|270||ZR|116||||||||||||0||||||BT|CHENE|5349
// |||||||000001|22/12/2016|Vente|58800,00||||B070|LA GRANDE PRAIRIE|17500|CHAMPAGNAC|17|82||ZM|30||||||||||||0||||||T||11200

// |||||||000001|21/06/2016|Vente|5555,00||||B101|LE FIEF DE CHEZ VIGER|17500|OZILLAC|17|270||ZN|66||||||||||||0||||||T||8930
// |||||||000001|21/06/2016|Vente|5555,00||||B101|LE FIEF DE CHEZ VIGER|17500|OZILLAC|17|270||ZN|69||||||||||||0||||||T||870
// |||||||000001|21/06/2016|Vente|5555,00||||B101|LE FIEF DE CHEZ VIGER|17500|OZILLAC|17|270||ZN|70||||||||||||0||||||T||1310

// |||||||000001|28/06/2018|Vente|301980,00|33|||B038|LA FORET|17500|MEUX|17|233||ZN|146||||||||||||0|1|Maison||190|6|S||500
// |||||||000001|28/06/2018|Vente|301980,00|33|||B038|LA FORET|17500|MEUX|17|233||ZN|146||||||||||||0|1|Maison||190|6|AG||2644
// |||||||000001|28/06/2018|Vente|301980,00||||B038|LA FORET|17500|MEUX|17|233||ZN|150||||||||||||0||||||T||2856

      auto push_evf_t = [&](pbx::Tokens &tokens_, const std::string& line_)
      {
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
            ._17_address_city = tokens_.at(17), // 17 Commune - Libellé de la commune
         // ._add_state = std::atoi( tokens_.at(18).data()), // 18 Code département
                                                // 19 Code commune
                                                // 20 Préfixe de section
            ._21_section = tokens_.at(21),      // 21 Section
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
            ._36_type_local = tokens_.at(36),   // 36 Type local : Maison Appartement Dépendance
                                                // 37 Identifiant local
                                                //    1 : maison ; 2 : appartement ; 3 : dépendance (isolée) ; 4 : local industriel et commercial ou assimilés
            ._38_surface = (float)atof(tokens_.at(38).data()), // 38 Surface réelle
         // ._39_nopieces = tokens_.at(39),     // 39 Nombre de pièces principales
            ._40_code_nature_culture = tokens_.at(40), // 40 Code nature culture
                                                // 41 Nature culture spéciale
            ._42_terrain = (float)atof(tokens_.at(42).data()), // 42 Surface terrain - Contenance du terrain
          };
          // clang-format on

          // 20/07/2017
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

          if (!data._36_type_local.empty())
          {
            auto found = TYPE_LOCAL_36.find(data._36_type_local);
            if (found == TYPE_LOCAL_36.end())
            {
              std::cout << "data._36_type_local = " << data._36_type_local << AT << std::endl;
              std::cout << line_ << AT << std::endl;
              TYPE_LOCAL_36.insert(data._36_type_local);
            }
          }
          if (!data._40_code_nature_culture.empty())
          {
            auto found = CODE_NATURE_CULTURE_40.find(data._40_code_nature_culture);
            if (found == CODE_NATURE_CULTURE_40.end())
            {
              std::cout << "data._40_code_nature_culture = " << data._40_code_nature_culture << AT << std::endl;
              std::cout << line_ << AT << std::endl;
              CODE_NATURE_CULTURE_40.insert({data._40_code_nature_culture, "?"});
            }
          }

          // si on n'a pas le code postal, on skip
          if (data._16_address_postal_code == 0)
          {
            // std::cout << "missing postal code " << line_ << AT << std::endl;
            ++missing_postal_code;
          }
          // si on sait pas de quoi on parle, on skip
          else if (data._36_type_local.empty() && data._40_code_nature_culture.empty())
          {
            // std::cout << "36=40=0 types=empty " << line_ << AT << std::endl;
            ++missing_code;
          }
          // si on sait pas la surface, on skip
          else if (data._38_surface == 0 && data._42_terrain == 0)
          {
            // std::cout << "38=42=0 surface=0 " << line_ << AT << std::endl;
            ++missing_surface;
          }
          // si on n'a pas le prix, on skip
          else if (data._10_t31 <= 1)
          {
            // std::cout << "31=0 price=0 " << line_ << AT << std::endl;
            ++missing_price;
          }
          else
          {
            // on sait de quoi on parle, on a une surface et un prix, on ajoute a la DB
            DB.add_data(data);
          }
        };
        pbx::parse_csv_file(push_evf_t, str, '|');
      });

  uint64_t T1 = gettime();

  std::cout << "time       = " << (T1 - T0) / 1'000'000'000. << " sec" << AT << std::endl;
  std::cout << "total DB   = " << std::fixed << std::setw(9) << std::right << std::to_string(DB.total) << AT
            << std::endl;
  std::cout << "empty      = " << std::fixed << std::setw(9) << std::right << std::to_string(DB.empty) << AT
            << std::endl;
  std::cout << "postalcode=0 " << std::fixed << std::setw(9) << std::right << std::to_string(missing_postal_code) << AT
            << std::endl;
  std::cout << "code=0       " << std::fixed << std::setw(9) << std::right << std::to_string(missing_code) << AT
            << std::endl;
  std::cout << "surface=0    " << std::fixed << std::setw(9) << std::right << std::to_string(missing_surface) << AT
            << std::endl;
  std::cout << "price=0      " << std::fixed << std::setw(9) << std::right << std::to_string(missing_price) << AT
            << std::endl;
  std::cout << "total      = " << std::fixed << std::setw(9) << std::right
            << std::to_string(DB.total + missing_code + missing_surface + missing_price) << AT << std::endl;
  // std::cout << (float)DB.empty / (float)DB.total << AT << std::endl;

  uint64_t time = {};

  time = DB.process<17, 1000, 9500, 17000>("La Rochelle - CR", {}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 1000, 9500, 17000>("La Rochelle - CR", {"Maison"}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  // Nieul-sur-Mer      17137
  // l'Houmeau          17137
  // Marsilly           17137
  // Esnandes           17137
  // Puilboreau         17138
  // Saint-Xandre       17138
  // Dompierre-sur-Mer  17139
  // Lagord             17140
  // Périgny            17180
  // Villedoux          17230
  // Charron            17230
  // Aytré              17440
  time = DB.process<17, 1000, 9500, 17000, 17137, 17138, 17139, 17140, 17180, 17440, 17690>(
      "La Rochelle - CR - petite couronne", {}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 1000, 9500, 17000, 17230, 17540, 17220, 17290, 17340, 17450, 17870, 17300, 17730, 17780>(
      "La Rochelle - CR - grande couronne", {}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  // 17286;LES PORTES EN RE;17880;;LES PORTES EN RE;46.2420561008,-1.50055338015
  // 17318;ST CLEMENT DES BALEINES;17590;LE GILLIEUX;ST CLEMENT DES BALEINES;46.2363966414,-1.53823691013
  // 17019;ARS EN RE;17590;;ARS EN RE;46.2111995219,-1.51504354145
  // 17207;LOIX;17111;;LOIX;46.2206970303,-1.44742224343
  // 17121;LA COUARDE SUR MER;17670;;LA COUARDE SUR MER;46.2010718838,-1.4366169353
  // 17051;LE BOIS PLAGE EN RE;17580;;LE BOIS PLAGE EN RE;46.1825283655,-1.37625908104
  // 17369;ST MARTIN DE RE;17410;;ST MARTIN DE RE;46.1990908569,-1.36683779933
  // 17161;LA FLOTTE;17630;;LA FLOTTE;46.1793660462,-1.32163012232
  // 17360;STE MARIE DE RE;17740;;STE MARIE DE RE;46.1560367496,-1.32154377304
  // 17297;RIVEDOUX PLAGE;17940;;RIVEDOUX PLAGE;46.159061213,-1.28302289715
  time = DB.process<17, 1000, 9500, 17000, 17880, 17590, 17111, 17670, 17580, 17410, 17630, 17740, 17940>(
      "La Rochelle - CR - Ile de Re", {}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  // 17323;ST DENIS D OLERON;17650;;ST DENIS D OLERON;46.0234747885,-1.38577964477
  // 17337;ST GEORGES D OLERON;17190;SAUZELLE;ST GEORGES D OLERON;45.9762465553,-1.32432969206
  // 17385;ST PIERRE D OLERON;17310;;ST PIERRE D OLERON;45.9396931461,-1.30530935595
  // 17140;DOLUS D OLERON;17550;;DOLUS D OLERON;45.9096185439,-1.25750953748
  // 17093;LE CHATEAU D OLERON;17480;;LE CHATEAU D OLERON;45.8813853125,-1.2166234835
  // 17411;ST TROJAN LES BAINS;17370;;ST TROJAN LES BAINS;45.8319189499,-1.22713007046
  // 17485;LE GRAND VILLAGE PLAGE;17370;;LE GRAND VILLAGE PLAGE;45.8620787516,-1.24274513266
  time = DB.process<17, 1000, 9500, 17000, 17650, 17840, 17190, 17310, 17550, 17480, 17370>(
      "La Rochelle - CR - Ile d'Oleron", {}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 1000, 18000, 75014>("Paris 14", {"Appartement"}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 1000, 18000, 92500>("Rueil - Peri AH - Seine AV", {"Appartement"}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 0, 8500, 17500>("Jonzac", {"Maison"}, {});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  time = DB.process<17, 0, 8500, 17500>("Jonzac ZN ZE ZM", {}, {"P", "T"});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

  // time = DB.preview<17500>("Jonzac ZN ZE ZM", {"ZN","ZE","ZM"}, {}, {"P","T"});
  time = DB.preview<17500>("Jonzac ZN ZE ZM", {"ZN"}, {}, {"P", "T"});
  std::cout << "time  = " << (time) / 1'000. << " micro sec" << AT << std::endl;

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
// clang-format-9 -i dvf_app.cpp
// Demande de Valeur Foncière
// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 -I../libx -O3 dvf_app.cpp
// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
