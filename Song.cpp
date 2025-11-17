#include "Song.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

// ==========================
// 匿名命名空间内的工具函数
// ==========================
namespace {

std::string trim_copy(const std::string &s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

std::string to_lower_copy(const std::string &s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return res;
}

std::string join_tags(const std::vector<std::string> &tags) {
    std::ostringstream oss;
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << tags[i];
    }
    return oss.str();
}

} // namespace

// ==========================
// 静态成员初始化
// ==========================
int Song::next_id_ = 1;

// ==========================
// 构造函数
// ==========================
Song::Song(const std::string &title, const std::string &artist,
           int duration_sec, int rating) {
    std::string t = trim_copy(title);
    std::string a = trim_copy(artist);

    if (t.empty()) {
        std::cout << "[错误] 标题不能为空\n";
        return;
    }
    if (a.empty()) {
        std::cout << "[错误] 艺人不能为空\n";
        return;
    }
    if (duration_sec <= 0) {
        std::cout << "[错误] 时长必须为正整数（秒）\n";
        return;
    }
    if (rating < 1 || rating > 5) {
        std::cout << "[错误] 评分必须在 1...5 之间\n";
        return;
    }

    id_ = next_id_++;
    title_ = t;
    artist_ = a;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}

// ==========================
// Setters
// ==========================
bool Song::set_title(const std::string &t) {
    std::string v = trim_copy(t);
    if (v.empty()) {
        std::cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = v;
    return true;
}

bool Song::set_artist(const std::string &a) {
    std::string v = trim_copy(a);
    if (v.empty()) {
        std::cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = v;
    return true;
}

bool Song::set_duration(int sec) {
    if (sec <= 0) {
        std::cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        std::cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}

// ==========================
// 标签管理
// ==========================
bool Song::add_tag(const std::string &tag) {
    std::string t = trim_copy(tag);
    if (t.empty()) {
        std::cout << "[提示] 空标签已忽略\n";
        return false;
    }
    std::string lower_t = to_lower_copy(t);
    for (const auto &existing : tags_) {
        if (to_lower_copy(existing) == lower_t) {
            std::cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }
    tags_.push_back(t);
    return true;
}

bool Song::remove_tag(const std::string &tag) {
    std::string key = to_lower_copy(trim_copy(tag));
    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == key) {
            tags_.erase(tags_.begin() + i);
            return true;
        }
    }
    std::cout << "[提示] 未找到该标签\n";
    return false;
}

// ==========================
// 关键词匹配
// ==========================
bool Song::matches_keyword(const std::string &kw) const {
    std::string k = to_lower_copy(trim_copy(kw));
    if (k.empty()) return false;

    std::string t = to_lower_copy(title_);
    std::string a = to_lower_copy(artist_);
    if (t.find(k) != std::string::npos || a.find(k) != std::string::npos)
        return true;

    for (const auto &tag : tags_) {
        if (to_lower_copy(tag).find(k) != std::string::npos)
            return true;
    }
    return false;
}

// ==========================
// 输出运算符重载
// ==========================
std::ostream &operator<<(std::ostream &os, const Song &s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_
       << " (" << s.duration_sec_ << "s) "
       << std::string(s.rating_, '*');
    if (!s.tags_.empty()) {
        os << " [tags: " << join_tags(s.tags_) << "]";
    }
    return os;
}

// ==========================
// 排序规则
// ==========================
bool operator<(const Song &a, const Song &b) {
    if (a.rating_ > b.rating_) return true;
    if (a.rating_ < b.rating_) return false;
    if (a.title_ < b.title_) return true;
    if (a.title_ > b.title_) return false;
    return a.id_ < b.id_;
}