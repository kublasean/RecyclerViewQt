# RecyclerViewQt
Android style RecyclerView implemented in C++ as a Qt widget

Inspired by https://stackoverflow.com/a/65338926

## Why?

To have a performant way to create a scrollable list of arbitrarily complex widgets

Qt only has two built-in options for lists of items:
* [QScrollArea](https://doc.qt.io/qt-5/qscrollarea.html#details) with its widget set to have a layout
  * No built in [Model/View](https://doc.qt.io/qt-5/model-view-programming.html)
  * Can fill Layout with whatever widgets you want
  * All widgets are created at once and persist out of view
  * Rendering is optimized and scrolling is smooth once initialized
* [QListView](https://doc.qt.io/qt-5/qlistview.html#details)
  * More performant
  * Works with [Model/View](https://doc.qt.io/qt-5/model-view-programming.html)
  * Limited UI possiblities with default delegate
  * Difficult to use custom painting with a custom delegate to create complex controls

## Proposed solution: RecyclerView
* Lazily create widgets as they scroll into view
* Reuse widgets that go out of view for widgets that come into view
* Possible to use with [Model/View](https://doc.qt.io/qt-5/model-view-programming.html)

Basically, copy architecture from [Android's RecyclerView](https://developer.android.com/guide/topics/ui/layout/recyclerview?gclid=Cj0KCQiA09eQBhCxARIsAAYRiynYs3upd3plIBZNh15JP1KdaGnArQgN68UoZ_6aQ1z-qat5vBDpVGEaArioEALw_wcB&gclsrc=aw.ds)

> RecyclerView makes it easy to efficiently display large sets of data. You supply the data and define how each item looks, and the RecyclerView library dynamically creates the elements when they're needed.

> As the name implies, RecyclerView recycles those individual elements. When an item scrolls off the screen, RecyclerView doesn't destroy its view. Instead, RecyclerView reuses the view for new items that have scrolled onscreen. This reuse vastly improves performance, improving your app's responsiveness and reducing power consumption.


