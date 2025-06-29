/* SPDX-License-Identifier: MIT */
/*
 * Author: Jianhui Zhao <zhaojh329@gmail.com>
 */

import { createI18n } from 'vue-i18n'

const i18n = createI18n({
  locale: 'en',
  fallbackLocale: 'en',
  silentTranslationWarn: true,
  silentFallbackWarn: true,
  messages: {
    'en': {},
    'zh-CN': {},
    'zh-TW': {},
    'ja-JP': {}
  }
})

export default i18n
