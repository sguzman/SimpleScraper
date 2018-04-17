#include <iostream>
#include <cstdlib>
#include <cpr/cpr.h>
#include <gq/Node.h>
#include <gq/Document.h>

constexpr static const unsigned short cores = 8;
constexpr static const unsigned short maxPage = 1263;
constexpr static const char base[]{"https://it-eb.com/page/"};

static inline const std::string page(const unsigned short num) noexcept {
  static const std::string gimmeThatBase{base};
  return gimmeThatBase + std::to_string(num);
}

static inline const std::string get(const char* url) noexcept {
  return cpr::Get(cpr::Url{url}).text;
}

static inline const std::string getPage(const unsigned short num) noexcept {
  return get(page(num).c_str());
}

static inline void links(const unsigned short i) noexcept {
  const auto&& html = getPage(i);

  CDocument doc;
  doc.parse(html);

  CSelection c = doc.find("main#main > div.container-outer > div.container > div.content > div.content-inner.standard-view > article");
  std::cout << "On page " << i << std::endl;
  for (auto&& j = 0u; j < c.nodeNum(); j++) {
    std::cout << c.nodeAt(j).childAt(1).childAt(3).childAt(1).childAt(1).childAt(1).attribute("href") << std::endl;
  }
}

static inline void threadLogic(const unsigned short start) noexcept {
  for (unsigned short i{start}; i < maxPage; i += cores) {
    links(i);
  }
}

int main() noexcept {
  static std::thread ts[cores];

  for (unsigned short i{0}; i < cores; ++i) {
    ts[i] = std::thread{threadLogic, i + 1};
  }

  for (unsigned short i{0}; i < cores; ++i) {
    std::cout << "Waiting on ts[" << i << ']' << std::endl;
    ts[i].join();
  }

  return EXIT_SUCCESS;
}