
#include "rcoinbase.h"
#include "reconstructor.h"
#include <boost/uuid/uuid_io.hpp>


#include "date.h"
namespace oberon {

namespace R {

DataFrame RCoinbase::run(const Rcpp::DataFrame & data) {
  using oberon::core::Duration;
  using oberon::core::Reconstructor;
  using oberon::core::Timestamp;
  using namespace date;
  Rcpp::CharacterVector type = data["_type"];
  Rcpp::CharacterVector side = data["_side"];
  Rcpp::CharacterVector reason = data["_reason"];
  Rcpp::NumericVector timestamp = data["_time"];
  Rcpp::NumericVector price = data["_price"];
  Rcpp::NumericVector remaining_size = data["_remaining_size"];
  Rcpp::NumericVector size = data["_size"];
  Rcpp::CharacterVector order_id = data["_order_id"];
  auto coinbase =
      Reconstructor::create(oberon::core::kCoinbase, "BTCUSD", this, 2.0);
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
    }
  }
  // Final elapsed for cleanup
  using boost::property_tree::ptree;
  ptree tree;
  tree.put("type", "elapsed");
  tree.put("time", format("%FT%TZ\n", current_timestamp + std::chrono::seconds {2} ));
  coinbase->process(tree);
  return Rcpp::DataFrame::create(Rcpp::Named("order_id") = order_id_,
                                 Rcpp::Named("timestamp") = timestamp_);
}

void RCoinbase::transmit( std::unique_ptr<oberon::core::Event> && event) {
  if(event) {
    order_id_.push_back(to_string(event->getOrderId()));
    timestamp_.push_back(event->getTimestamp().time_since_epoch().count()/1000000.0);
    event_no_.push_back(event->getEventNo());
    price_.push_back(event->getPrice());
    volume_.push_back(event->getVolume());
    trade_id_.push_back(to_string(event->getTradeId()));
    event_type_.push_back(event->getEventType());
  }
}


} // namespace oberon::R

} // namespace oberon
