declare namespace OUI {
  interface IOui {
    state: {
      locale: string;
      theme: string;
      hostname: string;
    };
    call(mod: string, func: string, param: any): Promise<any>;
    ubus(func: string, method: string, param: any): Promise<any>;
    login(user: string, passwd: string): Promise<void>;
    logout(): Promise<void>;
    setLocale(locale: string): Promise<void>;
    setTheme(theme: string): Promise<void>;
    setHostname(name: string): Promise<void>;
    reloadConfig(configName: string): Promise<any>;
    reconnect(delay: number | null | undefined): Promise<void>;
  }

  /* copied from @types/js-md5 */
  type message = string | any[] | Uint8Array | ArrayBuffer;
  interface Md5 {
    array(): number[];
    arrayBuffer(): ArrayBuffer;
    buffer(): ArrayBuffer;
    digest(): number[];
    hex(): string;
    toString(): string;
    update(message: message): Md5;
    base64(): string;
  }
  interface IMd5 {
    (message: message): string;
    hex(message: message): string;
    array(message: message): number[];
    digest(message: message): number[];
    arrayBuffer(message: message): ArrayBuffer;
    buffer(message: message): ArrayBuffer;
    create(): Md5;
    update(message: message): Md5;
    base64(message: message): string;
  }

  type timercb = () => void;
  type TimerOption = {
    time: number;
    autostart: boolean;
    immediate: boolean;
    repeat: boolean;
  }
  interface ITimer {
    create(name: string, callback: timercb, options: TimerOption): void;
    start(name: string): void;
    stop(name: string): void;
  }
}
export default OUI;
