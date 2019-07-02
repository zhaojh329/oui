# 组件

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
| 参数    | 说明        | 类型      | 可选值       | 默认值  |
|-------- |------------ |---------- |-------------|-------- |
| title   | 标题 | string | — | — |
| name    | 唯一标识 | string | — | — |

## uci-option

`uci-option`组件的所有属性被其它`uci-option-xx`组件所继承。

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| label       | 标签 | string | — | — |
| name        | 名称 | string | — | — |
| uci-option  | uci选项名称(如果提供该属性，则不再以name属性作为uci选项名称) | string | — | — |
| description | 对该选项的一个简短描述 | string | — | — |
| required    | 是否必填 | boolean | — | false |
| initial     | 初始值 | string/number | — | — |
| depend      | 依赖 | string | — | — |
| rules       | 表单验证规则 | string/object/Function(value) | — | — |
| load        | 自定义uci加载方式 | string/array/Function(sid, self) | — | — |
| save        | 自定义uci保存函数 | string/Function(sid, value, self) | — | — |
| apply       | 表单提交时的钩子函数 | Function(value, self) | — | — |
| tab         | 明确该选项所属的Tab面板 | string | — | — |
| header      | 自定义表格的列标题 | string | — | — |
| width       | 表格的列宽度 | string/number | — | — |

### 作用域插槽
| 名称 | 说明     |
|------|----------|
| —    |自定义选项内容，参数为 {sid, prop, value, self} |

## uci-option-dummy

仅用于显示值，不能编辑。

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |

## uci-option-input

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| placeholder | 占位符 | string | — | — |
| password | 密码框 | boolean | — | false |
| append | 追加在输入框尾部的字符串 | string | — | — |

## uci-option-switch

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| initial | 初始值 | string/number/boolean | — | — |
| active-value | switch打开时的值 | string/number/boolean | — | true |
| inactive-value | switch关闭时的值 | string/number/boolean | — | false |

## uci-option-list

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| initial | 初始值 | string/number/array | — | — |
| options | 可选列表值 | array | — | —  |
| multiple | 多选 | boolean | — | false |
| allow-create | 是否允许用户创建新条目 | boolean | — | false |
| exclude | 排除列表中的值 | string/Function(sid, self) | — | — |

## uci-option-dlist

### 属性
| 参数        | 说明        | 类型      | 可选值       | 默认值  |
|------------ |------------ |---------- |-------------|-------- |
| initial | 初始值 | array | — | — |