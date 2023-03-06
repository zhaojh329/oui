import OUI from '../oui_types'

declare module '@vue/runtime-core' {
  interface ComponentCustomProperties {
    $oui: OUI.IOui
    $md5: OUI.IMd5
    $timer: OUI.ITimer
  }
}

export {}
