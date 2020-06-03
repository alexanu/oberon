
#include "reconstructor_impl.h"
#include <boost/uuid/string_generator.hpp>

#include <boost/uuid/nil_generator.hpp>


#include <iostream>
#include "date.h"
namespace oberon {

namespace core {

bool PriceSide::operator <(const PriceSide & rhs) const {
if (side_ == rhs.side_)
  return price_ < rhs.price_;
else {
  if (side_ == kBid)
    return true;
  else
    return false;
}
}

//By default, toEvent() returns 0 Events. A derived class that overrides this method is supposed to return 1 Event
std::unique_ptr<Event> MessageHandler::Message::toEvent() {
  return std::unique_ptr<Event>();
}

MessageHandler::Message::~Message() {
}

PriceSide MessageHandler::ExchangeMessage::getPriceSide() {
return price_side_;
}

bool MessageHandler::Filled::accept(MessageHandler* mh) {
  return mh->filled();
  
}

bool MessageHandler::Opened::accept(MessageHandler* mh) {
  return mh->opened();
  
}

bool MessageHandler::FullyCancelled::accept(MessageHandler * mh) {
  return mh->fullyCancelled();
}

bool MessageHandler::Received::accept(MessageHandler * mh) {
  return mh->received();
}

bool MessageHandler::Elapsed::accept( MessageHandler * mh) {
  return mh->elapsed();
}

MessageHandler::Elapsed::Elapsed(const Timestamp & exchange_time) {
  timestamp_ = exchange_time;
}

MessageHandler::Elapsed::~Elapsed() {
  std::cout << "Elapsed deleted\n";
}

bool MessageHandler::Era::accept(MessageHandler* mh) {
}

MessageHandler::Era::Era() {
}

MessageHandler::Era::~Era() {
}

vector<std::unique_ptr<MessageHandler::Message>> MessageHandler::handle(vector<std::unique_ptr<MessageHandler::Message>> && messages) {
  for(auto& msg: messages) {
    received_ = std::move(msg);
    received_->accept(this);
  }
  return std::move(output_);
}

vector<std::unique_ptr<MessageHandler::Message>> MessageHandler::handle(std::unique_ptr<MessageHandler::Message> && message) {
  received_ = std::move(message);
  received_->accept(this);
  return std::move(output_);
  
}

Dedup::AnyState::~AnyState() {
}

// perform the 'do activity'
void Dedup::AnyState::_do(Dedup &) {
}

// the current state doesn't manage the event filled, give it to the upper state
void Dedup::AnyState::filled(Dedup & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->filled(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition filled not expected" << std::endl;
#endif
}

// the current state doesn't manage the event cancelled, give it to the upper state
void Dedup::AnyState::cancelled(Dedup & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->cancelled(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition cancelled not expected" << std::endl;
#endif
}

// the current state doesn't manage the event stop, give it to the upper state
void Dedup::AnyState::stop(Dedup & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->stop(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition stop not expected" << std::endl;
#endif
}

// the current state doesn't manage the event message, give it to the upper state
void Dedup::AnyState::message(Dedup & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->message(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition message not expected" << std::endl;
#endif
}

void Dedup::AnyState::create(Dedup &) {
}

// the current state doesn't manage the event elapsed, give it to the upper state
void Dedup::AnyState::elapsed(Dedup & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->elapsed(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition elapsed not expected" << std::endl;
#endif
}

Dedup::Dedup_State::Pristine_State::~Pristine_State() {
}

// to manage the event filled
void Dedup::Dedup_State::Pristine_State::filled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition elapsed" << std::endl;
#endif
  stm.deferExchangeMessage(true);
      stm._dedup_state._contaminated_state._doentry(stm);
      stm._set_currentState(stm._dedup_state._contaminated_state._fills_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Contaminated.Fills" << std::endl;
#endif
    }
}

// to manage the event cancelled
void Dedup::Dedup_State::Pristine_State::cancelled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition cancelled" << std::endl;
#endif
  stm.deferExchangeMessage(true);
      stm._dedup_state._contaminated_state._doentry(stm);
      stm._set_currentState(stm._dedup_state._contaminated_state._cancellations_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Contaminated.Cancellations" << std::endl;
#endif
    }
}

// to manage the event stop
void Dedup::Dedup_State::Pristine_State::stop(Dedup & stm) {
    {
      stm._set_currentState(stm._dedup_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup" << std::endl;
#endif
      stm._final();
    }
}

// to manage the event message
void Dedup::Dedup_State::Pristine_State::message(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition message" << std::endl;
#endif
  stm.output_.push_back(std::move(stm.received_));
    }
}

// to manage the event create
void Dedup::Dedup_State::Pristine_State::create(Dedup & stm) {
  	_doentry(stm);
}

// perform the 'entry behavior'
void Dedup::Dedup_State::Pristine_State::_doentry(Dedup & stm) {
#ifdef VERBOSE_STATE_MACHINE
    std::cout << "DEBUG : execute entry behavior of .Dedup.Pristine" << std::endl;
#endif
  stm.contamination_period_[0] = Timestamp();
  stm.contamination_period_[1] = Timestamp();
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::Pristine_State::_upper(Dedup & stm) {
    return &stm._dedup_state;
}

Dedup::Dedup_State::Contaminated_State::Fills_and_Cancellations_State::~Fills_and_Cancellations_State() {
}

// to manage the event elapsed
void Dedup::Dedup_State::Contaminated_State::Fills_and_Cancellations_State::elapsed(Dedup & stm) {
    if (stm.received_->getTimestamp() > stm.contamination_period_[1] + stm.wait_duration_
  ) {
      stm._dedup_state._contaminated_state._doexit(stm);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition elapsed, cleanse" << std::endl;
#endif
  stm.cleanseMessages();    stm._set_currentState(stm._dedup_state._pristine_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Pristine" << std::endl;
#endif
      stm._dedup_state._pristine_state.create(stm);
    }
}

// to manage the event filled
void Dedup::Dedup_State::Contaminated_State::Fills_and_Cancellations_State::filled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition <transition>" << std::endl;
#endif
  stm.deferExchangeMessage(true);  }
}

// to manage the event cancelled
void Dedup::Dedup_State::Contaminated_State::Fills_and_Cancellations_State::cancelled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition <transition>" << std::endl;
#endif
  stm.deferExchangeMessage(true);  }
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::Contaminated_State::Fills_and_Cancellations_State::_upper(Dedup & stm) {
    return &stm._dedup_state._contaminated_state;
}

Dedup::Dedup_State::Contaminated_State::Fills_State::~Fills_State() {
}

// to manage the event cancelled
void Dedup::Dedup_State::Contaminated_State::Fills_State::cancelled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition cancelled" << std::endl;
#endif
  stm.deferExchangeMessage(true);    stm._set_currentState(stm._dedup_state._contaminated_state._fills_and_cancellations_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Contaminated.Fills and Cancellations" << std::endl;
#endif
    }
}

// to manage the event filled
void Dedup::Dedup_State::Contaminated_State::Fills_State::filled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition <transition>" << std::endl;
#endif
  stm.deferExchangeMessage(true);  }
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::Contaminated_State::Fills_State::_upper(Dedup & stm) {
    return &stm._dedup_state._contaminated_state;
}

Dedup::Dedup_State::Contaminated_State::Cancellations_State::~Cancellations_State() {
}

// to manage the event filled
void Dedup::Dedup_State::Contaminated_State::Cancellations_State::filled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition filled" << std::endl;
#endif
  stm.deferExchangeMessage(true);    stm._set_currentState(stm._dedup_state._contaminated_state._fills_and_cancellations_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Contaminated.Fills and Cancellations" << std::endl;
#endif
    }
}

// to manage the event cancelled
void Dedup::Dedup_State::Contaminated_State::Cancellations_State::cancelled(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition <transition>" << std::endl;
#endif
  stm.deferExchangeMessage(true);  }
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::Contaminated_State::Cancellations_State::_upper(Dedup & stm) {
    return &stm._dedup_state._contaminated_state;
}

Dedup::Dedup_State::Contaminated_State::~Contaminated_State() {
}

// to manage the event elapsed
void Dedup::Dedup_State::Contaminated_State::elapsed(Dedup & stm) {
    if (stm.received_->getTimestamp() <= stm.contamination_period_[1] + stm.wait_duration_
  ) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition elapsed, ignored" << std::endl;
#endif
  //do nothing
    }
    else {
      stm._dedup_state._contaminated_state._doexit(stm);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition elapsed, pass" << std::endl;
#endif
  stm.passDeferred();    stm._set_currentState(stm._dedup_state._pristine_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Pristine" << std::endl;
#endif
      stm._dedup_state._pristine_state.create(stm);
    }
}

// perform the 'entry behavior'
void Dedup::Dedup_State::Contaminated_State::_doentry(Dedup & stm) {
#ifdef VERBOSE_STATE_MACHINE
    std::cout << "DEBUG : execute entry behavior of .Dedup.Contaminated" << std::endl;
#endif
  stm.contamination_period_[0] = stm.received_->getTimestamp();
  stm.contamination_period_[1] = stm.contamination_period_[0];
  stm.orchestrator_->setOutputBoundary(stm.contamination_period_[0]);
}

// perform the 'exit behavior'
void Dedup::Dedup_State::Contaminated_State::_doexit(Dedup & stm) {
#ifdef VERBOSE_STATE_MACHINE
    std::cout << "DEBUG : execute exit behavior of .Dedup.Contaminated" << std::endl;
#endif
  stm.orchestrator_->releaseOutputBoundary(stm.contamination_period_[0]);
  stm.contamination_period_[0] = Timestamp();
  stm.contamination_period_[1] = Timestamp();
  
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::Contaminated_State::_upper(Dedup & stm) {
    return &stm._dedup_state;
}

// to manage the event message
void Dedup::Dedup_State::Contaminated_State::message(Dedup & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition message" << std::endl;
#endif
  ExchangeMessage *ptr = dynamic_cast<ExchangeMessage *>(stm.received_.release());
  assert(ptr);
  stm.deferred_.push_back(std::unique_ptr<ExchangeMessage>(ptr));
  ;
    }
}

Dedup::Dedup_State::~Dedup_State() {
}

// to manage the event create
void Dedup::Dedup_State::create(Dedup & stm) {
    {
      stm._set_currentState(stm._dedup_state._pristine_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .Dedup.Pristine" << std::endl;
#endif
      stm._dedup_state._pristine_state.create(stm);
    }
}

// returns the state containing the current
Dedup::AnyState * Dedup::Dedup_State::_upper(Dedup &) {
    return 0;
}

Dedup::Dedup() {
    _current_state = 0;
}

Dedup::Dedup(DedupOrchestrator * orchestrator) : Dedup() {
  orchestrator_ = orchestrator;
}

Dedup::~Dedup() {
  // std::cout << "Dedup deleted\n";
}

// the operation you call to signal the event elapsed
bool Dedup::elapsed() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger elapsed" << std::endl;
#endif
      _current_state->elapsed(*this);
    }
    return (_current_state != 0);
}

bool Dedup::received() {
  return message();
}

bool Dedup::opened() {
  return message();
}

bool Dedup::volumeIncremented() {
  return message();
}

bool Dedup::priceAdvanced() {
  return message();
}

bool Dedup::priceReceded() {
  return message();
}

// the operation you call to signal the event filled
bool Dedup::filled() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger filled" << std::endl;
#endif
      _current_state->filled(*this);
    }
    return (_current_state != 0);
}

bool Dedup::partiallyCancelled() {
  return cancelled();
}

bool Dedup::fullyCancelled() {
  return cancelled();
}

void Dedup::deferExchangeMessage(bool update_contaminated_period) {
}

void Dedup::cleanseMessages() {
}

void Dedup::passDeferred() {
}

// the operation you call to signal the event cancelled
bool Dedup::cancelled() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger cancelled" << std::endl;
#endif
      _current_state->cancelled(*this);
    }
    return (_current_state != 0);
}

// the operation you call to signal the event stop
bool Dedup::stop() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger stop" << std::endl;
#endif
      _current_state->stop(*this);
    }
    return (_current_state != 0);
}

// the operation you call to signal the event message
bool Dedup::message() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger message" << std::endl;
#endif
      _current_state->message(*this);
    }
    return (_current_state != 0);
}

// the operation you call to signal the event create
bool Dedup::create() {
  if (_current_state == 0)
    (_current_state = &(*this)._dedup_state)->create(*this);
  return (_current_state != 0);
}

// to execute the current state 'do activity'
void Dedup::doActivity() {
    _current_state->_do(*this);
}

// execution done, internal
void Dedup::_final() {
    _current_state = 0;
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : final state reached" << std::endl;
#endif
}

DedupOrchestrator::AnyState::~AnyState() {
}

// perform the 'do activity'
void DedupOrchestrator::AnyState::_do(DedupOrchestrator &) {
}

// the current state doesn't manage the event stop, give it to the upper state
void DedupOrchestrator::AnyState::stop(DedupOrchestrator & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->stop(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition stop not expected" << std::endl;
#endif
}

// the current state doesn't manage the event message, give it to the upper state
void DedupOrchestrator::AnyState::message(DedupOrchestrator & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->message(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition message not expected" << std::endl;
#endif
}

// the current state doesn't manage the event elapsed, give it to the upper state
void DedupOrchestrator::AnyState::elapsed(DedupOrchestrator & stm) {
    AnyState * st = _upper(stm);
  
    if (st != 0)
      st->elapsed(stm);
#ifdef VERBOSE_STATE_MACHINE
    else
      std::cout << "DEBUG : transition elapsed not expected" << std::endl;
#endif
}

void DedupOrchestrator::AnyState::create(DedupOrchestrator &) {
}

DedupOrchestrator::DedupOrchestrator_State::Wait_State::~Wait_State() {
}

// to manage the event stop
void DedupOrchestrator::DedupOrchestrator_State::Wait_State::stop(DedupOrchestrator & stm) {
    {
      stm._set_currentState(stm._deduporchestrator_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .DedupOrchestrator" << std::endl;
#endif
      stm._final();
    }
}

// to manage the event message
void DedupOrchestrator::DedupOrchestrator_State::Wait_State::message(DedupOrchestrator & stm) {
    {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : execute activity of transition message" << std::endl;
#endif
  ExchangeMessage *ptr = dynamic_cast<ExchangeMessage *>(stm.received_.release());
  assert(ptr);
  stm.episode_.push_back(std::unique_ptr<ExchangeMessage>(ptr));
    }
}

// to manage the event elapsed
void DedupOrchestrator::DedupOrchestrator_State::Wait_State::elapsed(DedupOrchestrator & stm) {
    {
      stm._set_currentState(stm._deduporchestrator_state._deduplicate_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .DedupOrchestrator.Deduplicate" << std::endl;
#endif
      stm._deduporchestrator_state._deduplicate_state.create(stm);
    }
}

// returns the state containing the current
DedupOrchestrator::AnyState * DedupOrchestrator::DedupOrchestrator_State::Wait_State::_upper(DedupOrchestrator & stm) {
    return &stm._deduporchestrator_state;
}

DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::~Deduplicate_State() {
}

bool DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::_completion(DedupOrchestrator & stm) {
    {
      stm._deduporchestrator_state._deduplicate_state._doexit(stm);
      stm._set_currentState(stm._deduporchestrator_state._wait_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .DedupOrchestrator.Wait" << std::endl;
#endif
      return (bool) 1;
    }
}

// to manage the event create
void DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::create(DedupOrchestrator & stm) {
  	_doentry(stm);
  	_completion(stm);
}

// perform the 'entry behavior'
void DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::_doentry(DedupOrchestrator & stm) {
#ifdef VERBOSE_STATE_MACHINE
    std::cout << "DEBUG : execute entry behavior of .DedupOrchestrator.Deduplicate" << std::endl;
#endif
  for (auto &msg : stm.episode_) {
    ExchangeMessage *emsg = dynamic_cast<ExchangeMessage *>(msg.get());
    Dedup *dedup;
    try {
      dedup = stm.dedups_.at(emsg->getPriceSide()).get();
    } catch (const std::out_of_range &) {
      auto new_dedup(std::make_unique<Dedup>(&stm));
      new_dedup->create();
      dedup = (stm.dedups_[emsg->getPriceSide()] = std::move(new_dedup)).get();
    }
    auto output = dedup->handle(std::move(msg));
    std::move(output.begin(), output.end(), std::back_inserter(stm.output_));
  }
  stm.episode_.clear();
}

// perform the 'exit behavior'
void DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::_doexit(DedupOrchestrator & stm) {
#ifdef VERBOSE_STATE_MACHINE
    std::cout << "DEBUG : execute exit behavior of .DedupOrchestrator.Deduplicate" << std::endl;
#endif
  // Dedup *dedup = stm.contaminated_iter_->second;
  // dedup->timeElapsed(exchange_timestamp_);
  // if (!dedup->is_contaminated_)
  //  stm.contaminated_.erase(stm.contaminated_iter_);
}

// returns the state containing the current
DedupOrchestrator::AnyState * DedupOrchestrator::DedupOrchestrator_State::Deduplicate_State::_upper(DedupOrchestrator & stm) {
    return &stm._deduporchestrator_state;
}

DedupOrchestrator::DedupOrchestrator_State::~DedupOrchestrator_State() {
}

// to manage the event create
void DedupOrchestrator::DedupOrchestrator_State::create(DedupOrchestrator & stm) {
    {
      stm._set_currentState(stm._deduporchestrator_state._wait_state);
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : current state is now .DedupOrchestrator.Wait" << std::endl;
#endif
    }
}

// returns the state containing the current
DedupOrchestrator::AnyState * DedupOrchestrator::DedupOrchestrator_State::_upper(DedupOrchestrator &) {
    return 0;
}

// the operation you call to signal the event message
bool DedupOrchestrator::message() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger message" << std::endl;
#endif
      _current_state->message(*this);
    }
    return (_current_state != 0);
}

// the operation you call to signal the event elapsed
bool DedupOrchestrator::elapsed() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger elapsed" << std::endl;
#endif
      _current_state->elapsed(*this);
    }
    return (_current_state != 0);
}

void DedupOrchestrator::setOutputBoundary(const Timestamp & timestamp) {
}

void DedupOrchestrator::releaseOutputBoundary(const Timestamp & timestamp) {
}

// the operation you call to signal the event stop
bool DedupOrchestrator::stop() {
    if (_current_state != 0) {
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : fire trigger stop" << std::endl;
#endif
      _current_state->stop(*this);
    }
    return (_current_state != 0);
}

// the operation you call to signal the event create
bool DedupOrchestrator::create() {
  if (_current_state == 0)
    (_current_state = &(*this)._deduporchestrator_state)->create(*this);
  return (_current_state != 0);
}

// to execute the current state 'do activity'
void DedupOrchestrator::doActivity() {
    _current_state->_do(*this);
}

// execution done, internal
void DedupOrchestrator::_final() {
    _current_state = 0;
#ifdef VERBOSE_STATE_MACHINE
      std::cout << "DEBUG : final state reached" << std::endl;
#endif
}

bool DedupOrchestrator::received() {
  return message();
}

bool DedupOrchestrator::opened() {
  return message();
}

bool DedupOrchestrator::volumeIncremented() {
  return message();
}

bool DedupOrchestrator::priceAdvanced() {
  return message();
  
}

bool DedupOrchestrator::priceReceded() {
  return message();
  
}

bool DedupOrchestrator::filled() {
  return message();
  
}

bool DedupOrchestrator::partiallyCancelled() {
  return message();
  
}

bool DedupOrchestrator::fullyCancelled() {
  return message();
  
}

void ReconstructorImplementation::process(const boost::property_tree::ptree & message) {
  save(cleanse(extract(message)));
  
}

CoinbaseReconstructor::CoinbaseOpen::CoinbaseOpen(const Id & order_id, const Timestamp & timestamp, const Price & price, const Volume & remaining_size, BookSide side) {
  order_id_ = order_id;
  timestamp_ = timestamp;
  price_side_.price_ = price;
  price_side_.side_ = side;
  remaining_size_ = remaining_size;
  std::cout << "CoinbaseOpen created\n";
}

//By default, toEvent() returns 0 Events. A derived class that overrides this method is supposed to return 1 Event
std::unique_ptr<Event> CoinbaseReconstructor::CoinbaseOpen::toEvent() {
  return std::make_unique<EventImp>(
      order_id_, timestamp_, 1, price_side_.price_,
      price_side_.side_ == kBid ? remaining_size_ : -remaining_size_, kActivation);
}

CoinbaseReconstructor::CoinbaseOpen::~CoinbaseOpen() {
  std::cout << "CoinbaseOpen deleted\n";
}

vector<std::unique_ptr<MessageHandler::Message>> CoinbaseReconstructor::extract(const boost::property_tree::ptree & tree) {
  using namespace date;
  using namespace std;
  boost::uuids::string_generator gen;
  string type = tree.get<string>("type");
  Timestamp timestamp;
  std::istringstream ss{tree.get<string>("time")};
  ss >> parse("%FT%TZ", timestamp);
  vector<unique_ptr<Message>> output;
  if (type == "elapsed") {
    output.push_back(make_unique<Elapsed>(timestamp));
  }
  if (type == "open") {
    output.push_back(make_unique<CoinbaseOpen>(
        gen(tree.get<string>("order_id")), timestamp, 100, 10, kBid));
  }
  return output;
  
}

CoinbaseReconstructor::CoinbaseReconstructor( Store * store) {
  store_ = store;
  deduplicator_ = std::make_unique<DedupOrchestrator>();
  deduplicator_->create();
}

EventImp::EventImp(const Id & order_id, const Timestamp & timestamp, const EventNo & event_no, const Price & price, const Volume & volume, EventType event_type) {
  order_id_ = order_id;
  timestamp_ = timestamp;
  event_no_ = event_no;
  price_ = price;
  volume_ = volume;
  event_type_ = event_type;
  boost::uuids::nil_generator gen;
  trade_id_ = gen();
}


} // namespace oberon::core

} // namespace oberon