import Vue from 'vue'
import { Layout, Sider, Header, Menu, Submenu, MenuItem, Dropdown, DropdownMenu, DropdownItem } from 'iview'
import { Button, Card, Form, FormItem, Input, Icon, Avatar } from 'iview'
import lang from 'iview/dist/locale/en-US'
import { locale } from 'iview'

locale(lang)

Vue.component('Layout', Layout)
Vue.component('Sider', Sider)
Vue.component('Header', Header)
Vue.component('Menu', Menu)
Vue.component('Submenu', Submenu)
Vue.component('MenuItem', MenuItem)
Vue.component('Dropdown', Dropdown)
Vue.component('DropdownMenu', DropdownMenu)
Vue.component('DropdownItem', DropdownItem)
Vue.component('Button', Button)
Vue.component('Card', Card)
Vue.component('Form', Form)
Vue.component('Form-item', FormItem)
Vue.component('Input', Input)
Vue.component('Icon', Icon)
Vue.component('Avatar', Avatar)

import 'iview/dist/styles/iview.css'
