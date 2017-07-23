# DebounceEvent change log

The format is based on [Keep a Changelog](http://keepachangelog.com/)
and this project adheres to [Semantic Versioning](http://semver.org/).

## [2.0.1] 2017-07-23
### Added
- Added the option to specify repeat delay in constructor (set it to 0 to disable double click but get immediate responses)

## [2.0.0] 2017-01-25
### Changed
- Reporting # of clicks and length instead of predefined events
- Callback signature changed to report click count and length

### Removed
- EVENT_SINGLE_CLICK, EVENT_DOUBLE_CLICK, EVENT_LONG_CLICK

## [1.1.0] 2017-01-22
### Added
- Added support for toggle switches

### Fixed
- Fix bug with callback initialization

### Changed
- Changed constructor signature

## [1.0.0] 2016-12-29
- Initial commit as independent library
