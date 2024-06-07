if (process.platform === 'win32') {
  exports.EventLog = require('node-gyp-build')(__dirname).EventLog;
} else {
  const warning = 'The windows-eventlog module only writes to the Windows Eventlog. Please check process.platform to ensure this method is only invoked on Windows.';
  exports.EventLog = class EventLog {
    constructor() {
      console.warn(warning);
    }

    log() {
      console.warn(warning);
    }

    logSync() {
      console.warn(warning);
    }
  }
}
