// Unit test code for Event::Connect

#include "test.hpp"
#include <iostream>

#include <subject.hpp>
#include <observer.hpp>

using namespace std;
using sigcxx::Slot;

Test::Test()
    : testing::Test() {
}

Test::~Test() {

}

/*
 *
 */
TEST_F(Test, connect_method_once) {
  Subject s;
  Observer o;

  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);

  ASSERT_TRUE((s.CountBindings() == 0) && (s.signal1().count_connections() == 1) && (o.CountBindings() == 1));
}

/*
 *
 */
TEST_F(Test, connect_method_4_times) {
  using namespace sigcxx;

  Subject s;
  Observer o;

  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&o, &Observer::OnTest1IntegerParam);

  s.emit_signal1(1);    // this should call 4 times

//  o.CountBindings(&Observer::OnTest1IntegerParam);

  Signal<int> signal;
  signal.Connect(&o, &Observer::OnTest1IntegerParam);
  signal.Emit(500);
  // signal.DisconnectAll(&o, &Observer::OnTest1IntegerParam);
  o.UnbindAll<Observer, int>(&Observer::OnTest1IntegerParam);
//  o.UnbindAll();

  ASSERT_TRUE(o.test1_count() == 4 &&
      s.signal1().count_connections() == 4 &&
      o.CountBindings() == 4);
}

/*
 * This testcase test the connectiong between events -- chain load
 */
TEST_F(Test, connect_event_once) {
  Subject s1;
  sigcxx::Signal<int> s2;
  Observer o;

  s2.Connect(&o, &Observer::OnTest1IntegerParam);
  s1.signal1().connect(s2);

  s1.emit_signal1(1);  // cause chain signal_base

  ASSERT_TRUE((o.test1_count() == 1) && (s1.signal1().count_connections() == 1) && (s2.CountBindings() == 1)
                  && (s2.CountConnections() == 1) && (o.CountBindings() == 1));
}

/*
 * Disconnect 1
 */
TEST_F(Test, disconnect_once) {
  Subject s;
  Observer c;

  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);
  s.signal1().connect(&c, &Observer::OnTest1IntegerParam);

  s.signal1().disconnect_once(&c, &Observer::OnTest1IntegerParam, 0);
  s.signal1().disconnect_once(&c, &Observer::OnTest1IntegerParam, -1);

  s.emit_signal1(1);

  ASSERT_TRUE(c.test1_count() == 1);
}

TEST_F(Test, selfconsumer) {
  Subject s;

  s.signal1().connect(&s, &Subject::OnTest1IntegerParam);

  s.emit_signal1(1);
  s.emit_signal1(2);

  ASSERT_TRUE(s.test1_count() == 2);
}

TEST_F(Test, event_chaining) {
  Subject s1;
  sigcxx::Signal<int> s2;
  Observer c;

  s1.signal1().connect(s2);
  s2.Connect(&c, &Observer::OnTest1IntegerParam);

  s1.emit_signal1(1);

  ASSERT_TRUE(c.CountBindings() == 1 && c.test1_count() == 1);
}

TEST_F(Test, delete_when_called) {
  Subject s;
  Observer c1;
  Observer c2;
  Observer c3;
  Observer *obj = new Observer;

  s.signal0().connect(&c1, &Observer::OnTest0);
  s.signal0().connect(obj, &Observer::OnTestDestroy);
  s.signal0().connect(&c2, &Observer::OnTest0);
  s.signal0().connect(&c3, &Observer::OnTest0);

  s.emit_signal0();

  ASSERT_TRUE((c1.test0_count() == 1) &&
      (c2.test0_count() == 1) &&
      (c3.test0_count() == 1) &&
      (s.signal0().count_connections() == 3));
}

TEST_F(Test, delete_more_when_called) {
  Subject s;
  Observer c1;
  Observer c2;
  Observer c3;
  Observer *obj1 = new Observer;
  Observer *obj2 = new Observer;
  Observer *obj3 = new Observer;

  s.signal0().connect(&c1, &Observer::OnTest0);
  s.signal0().connect(obj1, &Observer::OnTestDestroy);
  s.signal0().connect(&c2, &Observer::OnTest0);
  s.signal0().connect(&c3, &Observer::OnTest0);
  s.signal0().connect(obj2, &Observer::OnTestDestroy);
  s.signal0().connect(obj3, &Observer::OnTestDestroy);

  s.emit_signal0();

  ASSERT_TRUE((c1.test0_count() == 1) &&
      (c2.test0_count() == 1) &&
      (c3.test0_count() == 1) &&
      (s.signal0().count_connections() == 3));
}

TEST_F(Test, count_event_connections) {
  Subject s1;
  sigcxx::Signal<> s2;
  Observer c;

  s1.signal0().connect(s2);
  s1.signal0().connect(&c, &Observer::OnTest0);
  s1.signal0().connect(s2);

  ASSERT_TRUE(
      (s1.signal0().count_connections(s2) == 2) &&
          (s1.signal0().count_connections(&c, &Observer::OnTest0) == 1)
  );
}
