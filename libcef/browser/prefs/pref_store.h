// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_PREFS_PREF_STORE_H_
#define CEF_LIBCEF_BROWSER_PREFS_PREF_STORE_H_

#include <stdint.h>

#include <string>

#include "base/observer_list.h"
#include "components/prefs/persistent_pref_store.h"
#include "components/prefs/pref_value_map.h"

// Preference store implementation that supports explicit manipulation of the
// contents of the store, triggering notifications where appropriate.
// Based on components/prefs/testing_pref_store.h.
class CefPrefStore : public PersistentPrefStore {
 public:
  CefPrefStore();

  CefPrefStore(const CefPrefStore&) = delete;
  CefPrefStore& operator=(const CefPrefStore&) = delete;

  // Overriden from PrefStore.
  bool GetValue(base::StringPiece key,
                const base::Value** result) const override;
  base::Value::Dict GetValues() const override;
  void AddObserver(PrefStore::Observer* observer) override;
  void RemoveObserver(PrefStore::Observer* observer) override;
  bool HasObservers() const override;
  bool IsInitializationComplete() const override;

  // PersistentPrefStore overrides:
  bool GetMutableValue(const std::string& key, base::Value** result) override;
  void ReportValueChanged(const std::string& key, uint32_t flags) override;
  void SetValue(const std::string& key,
                base::Value value,
                uint32_t flags) override;
  void SetValueSilently(const std::string& key,
                        base::Value value,
                        uint32_t flags) override;
  void RemoveValuesByPrefixSilently(const std::string& prefix) override;
  void RemoveValue(const std::string& key, uint32_t flags) override;
  bool ReadOnly() const override;
  PrefReadError GetReadError() const override;
  PersistentPrefStore::PrefReadError ReadPrefs() override;
  void ReadPrefsAsync(ReadErrorDelegate* error_delegate) override;
  virtual void CommitPendingWrite(
      base::OnceClosure done_callback,
      base::OnceClosure synchronous_done_callback) override;
  void SchedulePendingLossyWrites() override;
  void ClearMutableValues() override;
  void OnStoreDeletionFromDisk() override;

  // Marks the store as having completed initialization.
  void SetInitializationCompleted();

  // Used for tests to trigger notifications explicitly.
  void NotifyPrefValueChanged(const std::string& key);
  void NotifyInitializationCompleted();

  // Some convenience getters/setters.
  void SetString(const std::string& key, const std::string& value);
  void SetInteger(const std::string& key, int value);
  void SetBoolean(const std::string& key, bool value);

  bool GetString(const std::string& key, std::string* value) const;
  bool GetInteger(const std::string& key, int* value) const;
  bool GetBoolean(const std::string& key, bool* value) const;

  // Determines whether ReadPrefsAsync completes immediately. Defaults to false
  // (non-blocking). To block, invoke this with true (blocking) before the call
  // to ReadPrefsAsync. To unblock, invoke again with false (non-blocking) after
  // the call to ReadPrefsAsync.
  void SetBlockAsyncRead(bool block_async_read);

  // Getter and Setter methods for setting and getting the state of the
  // |TestingPrefStore|.
  virtual void set_read_only(bool read_only);
  void set_read_success(bool read_success);
  void set_read_error(PersistentPrefStore::PrefReadError read_error);
  bool committed() { return committed_; }

 protected:
  ~CefPrefStore() override;

 private:
  // Stores the preference values.
  PrefValueMap prefs_;

  // Flag that indicates if the PrefStore is read-only
  bool read_only_;

  // The result to pass to PrefStore::Observer::OnInitializationCompleted
  bool read_success_;

  // The result to return from ReadPrefs or ReadPrefsAsync.
  PersistentPrefStore::PrefReadError read_error_;

  // Whether a call to ReadPrefsAsync should block.
  bool block_async_read_;

  // Whether there is a pending call to ReadPrefsAsync.
  bool pending_async_read_;

  // Whether initialization has been completed.
  bool init_complete_;

  // Whether the store contents have been committed to disk since the last
  // mutation.
  bool committed_;

  std::unique_ptr<ReadErrorDelegate> error_delegate_;
  base::ObserverList<PrefStore::Observer, true>::Unchecked observers_;
};

#endif  // COMPONENTS_PREFS_TESTING_PREF_STORE_H_
