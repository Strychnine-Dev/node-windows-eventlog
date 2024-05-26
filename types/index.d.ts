declare module '@strychnine-labs/windows-eventlog' {
  export class EventLog {
    /**
     * This creates an instance of the EventLog. You can optionally pass a logName, defaults to "Application".
     * 
     * @param logName Application log name, defaults to "Application".
     */
    constructor(logName?: string);

    log: LogOverloads;
    logSync: LogSyncOverloads;
  }

  type Severity = 'info' | 'warn' | 'error';

  interface LogOverloads {
    /**
     * This method will create an entry in the event log with the given message.
     * 
     * Optionally you can specify a severity. 
     * 
     * @param severity Log severity level. The possible values are "info", "warn" and "error", defaults to "info".
     * @param message Log message
     * @param callback Function to be called when the operation is complete
     */
    (severity: Severity, message: string, callback: (err?: Error) => void): void;

    /**
     * This method will create an entry in the event log with the given message with severity level "info".
     * 
     * @param message Log message
     * @param callback Function to be called when the operation is complete
     */
    (message: string, callback: (err?: Error) => void): void;
  }

  interface LogSyncOverloads {
    /**
     * This method will create an entry in the event log with the given message.
     * 
     * Optionally you can specify a severity. 
     * 
     * @param severity Log severity level. The possible values are "info", "warn" and "error", defaults to "info".
     * @param message Log message
     */
    (severity: Severity, message: string): void;

    /**
     * This method will create an entry in the event log with the given message with severity level "info".
     * 
     * @param message Log message
     */
    (message: string): void;
  }
}