import Vue from 'vue'
import {locale, Message, Layout, Sider, Header, Menu, Submenu, MenuItem, Dropdown, DropdownMenu, DropdownItem,
  Button, Card, Form, FormItem, Input, Icon, Avatar, Table, Breadcrumb, BreadcrumbItem, Tabs,
  TabPane, Select, Option, Switch, Spin, Upload, Modal} from 'iview'

import lang from 'iview/dist/locale/en-US'

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
Vue.component('FormItem', FormItem)
Vue.component('Input', Input)
Vue.component('Icon', Icon)
Vue.component('Avatar', Avatar)
Vue.component('Table', Table)
Vue.component('Breadcrumb', Breadcrumb)
Vue.component('BreadcrumbItem', BreadcrumbItem)
Vue.component('Tabs', Tabs)
Vue.component('TabPane', TabPane)
Vue.component('Select', Select)
Vue.component('Option', Option)
Vue.component('i-switch', Switch)
Vue.component('Upload', Upload)

Vue.prototype.$Message = Message;
Vue.prototype.$Spin = Spin;
Vue.prototype.$Modal = Modal;

import 'iview/dist/styles/iview.css'
