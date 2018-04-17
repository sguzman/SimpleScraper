#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <cpr/cpr.h>
#include <gq/Node.h>
#include <gq/Document.h>
#include <cpp_redis/cpp_redis>
#include <brotli/encode.h>
#include <brotli/decode.h>

namespace globals {
  constexpr static const unsigned short cores = 8;
  constexpr static const unsigned short maxPage = 1263;
  constexpr static const char base[]{"https://it-eb.com/page/"};
  constexpr static const char redisHashName[]{"ebooks"};
  static std::thread ts[globals::cores];
  cpp_redis::client client;
}

namespace net {
  static inline const std::string page(const unsigned short num) noexcept {
    static const std::string gimmeThatBase{globals::base};
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
    for (unsigned short i{start}; i < globals::maxPage; i += globals::cores) {
      links(i);
    }
  }
}

static inline const std::vector<std::string> keys(cpp_redis::client& client) noexcept {
  auto hashKeysFuture = client.hkeys(globals::redisHashName);
  {
    client.sync_commit();
  }

  auto hashKeysFutureVal = hashKeysFuture.get();
  if (!hashKeysFutureVal.is_array()) {
    std::cout << "Bad value for hash keys" << std::endl;
    exit(1);
  }

  const auto& hashKeysReply = hashKeysFutureVal.as_array();

  std::vector<std::string> buf{};
  for (auto iter = hashKeysReply.cbegin(); iter < hashKeysReply.cend(); ++iter) {
    if (!iter->is_string()) {
      std::cout << "Encountered non-string key in hash" << std::endl;
      exit(2);
    }

    buf.push_back(iter->as_string());
  }

  std::cout << "Found " << buf.size() << " Http cache entries" << std::endl;

  return buf;
}

static inline const std::vector<std::string> values(cpp_redis::client& client, const std::vector<std::string>& keys) noexcept {
  if (keys.empty()) {
    return std::vector<std::string>{};
  }

  auto hashValuesFuture = client.hmget(globals::redisHashName, keys);
  {
    client.sync_commit();
  }

  const auto& hashValuesReply = hashValuesFuture.get();
  if (!hashValuesReply.is_array()) {
    std::cout << "Non array found for values" << std::endl;
    exit(4);
  }

  const auto& hashValues = hashValuesReply.as_array();
  std::vector<std::string> buf{};
  for (auto iter = hashValues.cbegin(); iter < hashValues.cend(); ++iter) {
    if (!iter->is_string()) {
      std::cout << "Encountered non-string value in hash" << std::endl;
      exit(5);
    }

    buf.push_back(iter->as_string());
  }

  return buf;
}

template <typename A>
static inline std::ostream& print(const std::vector<A>& vec, std::ostream& os = std::cout) noexcept {
  os << '[';
  if (!vec.empty()) {
    os << vec.front();
  }

  for (auto iter = vec.cbegin() + 1; iter < vec.cend(); ++iter) {
    os << ", " << *iter;
  }

  return os << ']';
}

template <typename A, typename B>
static inline std::ostream& print(const std::unordered_map<A, B>& mappy, std::ostream& os = std::cout) noexcept {
  os << '[';
  if (!mappy.empty()) {
    os << '(' << mappy.cbegin()->first << ',' << mappy.cbegin()->second << ')';
  }

  for (auto iter = mappy.cbegin(); iter != mappy.cend(); ++iter) {
    if (iter == mappy.cbegin()) {
      continue;
    }

    os << ", " << '(' << iter->first << ',' << iter->second << ')';
  }

  return os << ']';
}

static inline const std::unordered_map<std::string, std::string> map(cpp_redis::client& client) noexcept {
  std::vector<std::string> hashKeys{keys(client)};
  std::vector<std::string> hashValues{values(client, hashKeys)};
  if (hashKeys.size() != hashValues.size()) {
    std::cout << "Keys do not match values" << std::endl;
    exit(5);
  }

  std::unordered_map<std::string, std::string> buf{};
  for (auto i = 0u; i < hashKeys.size(); ++i) {
    const auto &key = hashKeys[i];
    const auto &value = hashValues[i];
    buf[key] = value;
  }

  return buf;
}

int main() noexcept {
  globals::client.connect();

  const auto& hash = map(globals::client);
  print(hash) << std::endl;

  return EXIT_SUCCESS;
}