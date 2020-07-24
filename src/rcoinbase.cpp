
#include "rcoinbase.h"
#include "reconstructor.h"
#include <boost/uuid/uuid_io.hpp>


#include "date.h"
namespace oberon {

namespace R {

Rcpp::List RCoinbase::run(const Rcpp::DataFrame & data, bool extract_only) {
  using oberon::core::Duration;
  using oberon::core::Reconstructor;
  using oberon::core::Timestamp;
  using namespace date;
  Rcpp::CharacterVector type = data["_type"];
  Rcpp::CharacterVector side = data["_side"];
  Rcpp::CharacterVector reason = data["_reason"];
  Rcpp::NumericVector timestamp = data["_time"];
  Rcpp::NumericVector local_timestamp = data["local_timestamp"];
  Rcpp::NumericVector price = data["_price"];
  Rcpp::NumericVector remaining_size = data["_remaining_size"];
  Rcpp::NumericVector size = data["_size"];
  Rcpp::NumericVector trade_id = data["_trade_id"];
  Rcpp::CharacterVector order_id = data["_order_id"];
  Rcpp::CharacterVector maker_order_id = data["_maker_order_id"];
  Rcpp::CharacterVector taker_order_id = data["_taker_order_id"];
  auto coinbase = Reconstructor::create(oberon::core::kCoinbase, "BTCUSD", this,
                                        2.0, extract_only);
  oberon::core::Timestamp current_timestamp = Timestamp();
  for (size_t i = 0; i < type.size(); ++i) {
    Timestamp next_timestamp =
        Timestamp(Duration(lround(timestamp[i] * 1000000)));
    if (current_timestamp != next_timestamp) {
      using boost::property_tree::ptree;
      ptree tree;
      current_timestamp = next_timestamp;
      tree.put("type", "elapsed");
      tree.put("time", format("%FT%TZ\n", current_timestamp));
      coinbase->process(tree);
    }
    using boost::property_tree::ptree;
    ptree tree;
    tree.put("time", format("%FT%TZ\n", current_timestamp));
    tree.put("local_timestamp",
             format("%FT%TZ\n",
                    Timestamp(Duration(lround(local_timestamp[i] * 1000000)))));
    if (type[i] == "open") {
      tree.put("type", "open");
      tree.put("order_id", order_id[i]);
      tree.put("price", price[i]);
      tree.put("remaining_size", remaining_size[i]);
      tree.put("side", side[i]);
      coinbase->process(tree);
    } else if (type[i] == "done") {
      tree.put("type", "done");
      tree.put("order_id", order_id[i]);
      tree.put("price", price[i]);
      tree.put("remaining_size", remaining_size[i]);
      tree.put("reason", reason[i]);
      tree.put("side", side[i]);
      coinbase->process(tree);
    } else if (type[i] == "received") {
      tree.put("type", "received");
      tree.put("order_id", order_id[i]);
      // tree.put("order_type", order_type[i]);
      tree.put("price", price[i]);
      tree.put("size", size[i]);
      tree.put("side", side[i]);
      coinbase->process(tree);
    } else if (type[i] == "match") {
      tree.put("type", "match");
      tree.put("maker_order_id", maker_order_id[i]);
      tree.put("taker_order_id", taker_order_id[i]);
      tree.put("price", price[i]);
      tree.put("size", size[i]);
      tree.put("side", side[i]);
      tree.put("trade_id", trade_id[i]);
      coinbase->process(tree);
    }
  }
  // Final elapsed for cleanup
  using boost::property_tree::ptree;
  ptree tree;
  tree.put("type", "elapsed");
  tree.put("time",
           format("%FT%TZ\n", current_timestamp + std::chrono::seconds{2}));
  coinbase->process(tree);
  return toDataframe();
}


} // namespace oberon::R

} // namespace oberon
