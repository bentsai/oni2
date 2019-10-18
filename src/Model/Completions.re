/*
 * Completions.re
 *
 * This module is responsible for managing completion state
 */

open Oni_Core;
open Oni_Core.Types;
open Oni_Extensions;

type t = {
  // The last completion meet we found
  meet: option(Actions.completionMeet),
  completions: list(Actions.completionItem),
  filteredCompletions: list(Actions.completionItem),
  filter: option(string),
  selected: option(int),
};

let default: t = {
  meet: None,
  selected: None,
  filter: None,
  filteredCompletions: [{
    completionLabel: "log",
    completionKind: CompletionKind.Method,
    completionDetail: Some("() => ()"),
  }, {
    completionLabel: "warn",
    completionKind: CompletionKind.Method,
    completionDetail: None,
  }, {
    completionLabel: "error",
    completionKind: CompletionKind.Method,
    completionDetail: None,
  }],
  completions: [{
    completionLabel: "log",
    completionKind: CompletionKind.Method,
    completionDetail: Some("() => ()"),
  }, {
    completionLabel: "warn",
    completionKind: CompletionKind.Method,
    completionDetail: None,
  }, {
    completionLabel: "error",
    completionKind: CompletionKind.Method,
    completionDetail: None,
  }],
}

let isActive = (v: t) => {
  switch (v.meet) {
  | None => false
  | Some(_) => switch(v.filteredCompletions) {
  | [_hd, ..._tail] => true
  | _ => false
  }
  }
};

let endCompletions = (v: t) => {
  default
};

let startCompletions = (meet: Actions.completionMeet, v: t) => {
  ...v,
  meet: Some(meet),
  completions: default.completions,
  filteredCompletions: default.filteredCompletions,
};

let _applyFilter = (filter: option(string), items: list(Actions.completionItem)) => {
  switch (filter) {
  | None => items
  | Some(filter) =>
  let re = Str.regexp_string(filter);
  List.filter((item: Actions.completionItem) => {
    switch (Str.search_forward(re, item.completionLabel)) {
    | exception Not_found => false
    | _ => true
    }
  }, items);
  }
};

let filter = (filter: string, v: t) => {
  ...v,
  filter: Some(filter),
  filteredCompletions: _applyFilter(Some(filter), v.completions) |> Utility.firstk(5)
};

let setItems = (items: list(Actions.completionItem), v: t) => {
  ...v,
  completions: items,
  filteredCompletions: _applyFilter(v.filter, items),
};

let reduce = (v: t, action: Actions.t) => {
  let newV = switch (action) {
  | Actions.CompletionStart(meet) => startCompletions(meet, v)
  | Actions.CompletionSetItems(_meet, items) => setItems(items, v)
  | Actions.CompletionBaseChanged(base) => filter(base, v)
  | Actions.CompletionEnd => endCompletions(v)
  | _ => v
  }

  if (isActive(newV)) {
    switch (action) {
    | Actions.MenuNextItem => 
      print_endline ("NEXT");
      newV
    | Actions.MenuPreviousItem => 
      print_endline ("PREVIOUS");
      newV
    }
  } else {
    new
  }
}