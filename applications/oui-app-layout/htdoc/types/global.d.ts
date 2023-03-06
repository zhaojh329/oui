import OUI from '@types/oui'

declare module '@vue/runtime-core' {
  interface ComponentCustomProperties {
    $oui: OUI.IOui
    $timer: OUI.ITimer
    $md5: OUI.IMd5
  }
}

export {}
