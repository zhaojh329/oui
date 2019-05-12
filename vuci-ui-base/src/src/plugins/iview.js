import Vue from 'vue'
import { Button } from 'iview'
import lang from 'iview/dist/locale/en-US'
import { locale } from 'iview'

locale(lang)

Vue.component('Button', Button)

import 'iview/dist/styles/iview.css'
