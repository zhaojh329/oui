# 属性

## uci-form

### 属性
| 参数      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| config    | uci配置文件  | string   | — | — |
| tabbed    | 将每个uci-section渲染成Tab标签页 | boolean | — | false |
| after-loaded | uci配置加载完成后的钩子 | Function() | — | — |

## uci-section

### 属性
| 参数      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| type    | section类型  | string   | — | — |
| name    | section名称  | string   | — | — |
| title    | 标题  | string   | — | — |
| addable   | 是否可以添加或者删除 | boolean | — |  false |
| anonymous | 是否是匿名section，若为false，添加操作将添加一个命名section  | boolean   | — |  true |
| table     | 是否渲染成一个表格 | boolean | — |  false |
| sortable  | 是支持排序(table为true时有效) | boolean | — |  false |
| filter    | 过滤 | Function(s, self) | — | — |
| options   | 自定义选项 | object | — | — |
| collabsible | 是否可折叠 | boolean | — | true |
| teasers | 折叠时需要显示的选项名称(默认显示所有选项) | array | — | — |
| add | 自定义删除函数。返回sid或者Promise | Function(self) | — | — |
| before-del | 执行删除前的钩子,若返回 false 或者返回 Promise 且被 reject，则阻止删除| Function(sid, self) | — | — |

## uci-tab

### 属性
| 参数      | 说明        | 类型      | 可选值       | 默认值  |
|---------- |------------ |---------- |-------------|-------- |
| title | 标题 | string | — | — |
| name | 唯一标识 | string | — | — |
