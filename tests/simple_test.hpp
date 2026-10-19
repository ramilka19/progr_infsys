#pragma once
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct TestCase { std::string name; std::function<void()> body; };
inline std::vector<TestCase>& registry() { static std::vector<TestCase> tests; return tests; }
struct RegisterTest { RegisterTest(std::string name, std::function<void()> body) { registry().push_back({std::move(name), std::move(body)}); } };
#define TF_CONCAT_IMPL(a,b) a##b
#define TF_CONCAT(a,b) TF_CONCAT_IMPL(a,b)
#define TEST_CASE(name) static void TF_CONCAT(test_, __LINE__)(); static RegisterTest TF_CONCAT(reg_, __LINE__)(name, TF_CONCAT(test_, __LINE__)); static void TF_CONCAT(test_, __LINE__)()
#define CHECK_TRUE(expr) do { if(!(expr)) throw std::runtime_error(std::string("CHECK_TRUE failed: ") + #expr); } while(false)
#define CHECK_EQ(a,b) do { auto va=(a); auto vb=(b); if(!(va==vb)) throw std::runtime_error(std::string("CHECK_EQ failed: ") + #a + " == " + #b); } while(false)
#define CHECK_THROWS(expr) do { bool thrown=false; try { expr; } catch(...) { thrown=true; } if(!thrown) throw std::runtime_error(std::string("CHECK_THROWS failed: ") + #expr); } while(false)
inline int run_all_tests() { int failed=0; for(auto& test : registry()) { try { test.body(); std::cout << "[ OK ] " << test.name << '\n'; } catch(const std::exception& e) { ++failed; std::cerr << "[FAIL] " << test.name << ": " << e.what() << '\n'; } } std::cout << "Executed " << registry().size() << " tests, failed " << failed << '\n'; return failed == 0 ? 0 : 1; }
