#include <iostream>
#include <cstdlib>
#include <cpr/cpr.h>
#include <gq/Node.h>
#include <gq/Document.h>

constexpr static const unsigned short cores = 4;

static inline const std::string get(const char* url) noexcept {
  return cpr::Get(cpr::Url{url}).text;
}

int main() noexcept {
  const auto&& html = get("https://it-eb.com/");
  std::cout << cores << std::endl;

  CDocument doc;
  doc.parse(html);

  CSelection c = doc.find("li");
  std::cout << c.nodeAt(0).text() << std::endl;

  return EXIT_SUCCESS;
}