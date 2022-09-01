<template>
  <div>
    <n-card title="通信规则" style="margin-bottom: 16px">
      <n-data-table
        :key="(row) => row.key"
        :columns="columns"
        :data="tableData"
        :loading="loading"
      />
    </n-card>
    <n-modal v-model:show="showModal" preset="dialog" title="编辑规则">
      <n-form
        :model="model"
        label-placement="left"
        label-width="auto"
        require-mark-placement="right-hanging"
        size="large"
      >
        <n-form-item label="名称">
          <n-input v-model:value="model.name" />
        </n-form-item>
        <n-form-item label="网卡">
          <n-select v-model:value="model.src" :options="srcOption" />
        </n-form-item>
        <n-form-item label="IP">
          <n-select v-model:value="model.family" :options="familyOption" />
        </n-form-item>
        <n-form-item label="协议">
          <n-select v-model:value="model.proto" :options="protoOption" />
        </n-form-item>
        <n-form-item label="端口号">
          <n-input v-model:value="model.dest_port" />
        </n-form-item>
        <n-form-item label="动作">
          <n-select v-model:value="model.target" :options="targetOption" />
        </n-form-item>
      </n-form>
      <template #action>
        <n-button type="primary" @click="submit">确定</n-button>
      </template>
    </n-modal>
  </div>
</template>
<script setup>
import { h, reactive, toRefs, getCurrentInstance } from 'vue'
// 字典数据，下拉框选项
import { srcOption, familyOption, protoOption, targetOption } from '../dict'
const data = reactive({
  // 表格列配置
  columns: [
    {
      title: '名称',
      key: 'name'
    },
    {
      title: '网口',
      key: 'src'
    },
    {
      title: 'IP',
      key: 'family'
    },
    {
      title: '协议',
      key: 'proto'
    },
    {
      title: '端口',
      key: 'dest_port'
    },
    {
      title: '动作',
      key: 'target'
    },
    {
      title: '操作',
      key: 'handle',
      render: (row) => {
        return h(
          'div',
          { onClick: () => showDialog(row), class: 'edit-btn' },
          { default: () => '编辑' }
        )
      }
    }
  ],
  // 表格数据，防火墙规则
  tableData: [
    {
      key: 0,
      name: '',
      src: '',
      proto: '',
      dest_port: '',
      family: '',
      target: ''
    },
    {
      key: 1,
      name: '',
      src: '',
      proto: '',
      family: '',
      dest_port: '',
      target: ''
    },
    {
      key: 2,
      name: '',
      src: '',
      proto: '',
      family: '',
      target: ''
    }
  ],
  loading: true,
  // 模态框显示隐藏
  showModal: false,
  // form表单
  model: {},
  deepModel: {}
})

const showDialog = (row) => {
  data.showModal = true
  data.model = row
  data.deepModel = JSON.parse(JSON.stringify(row))
}
const hidenDialog = () => {
  data.showModal = false
  data.model = {}
  data.deepModel = {}
}
// 获取改变的，需要修改的params
const getUpdateParams = () => {
  const params = {}
  const keyList = Object.keys(data.model)
  keyList.forEach((key) => {
    if (data.model[key] !== data.deepModel[key]) {
      params[key] = data.model[key]
    }
  })
  params.key = data.model.key
  return params
}
// 获取this实例，以便使用$oui
const { proxy } = getCurrentInstance()
// 定义修改配置的方法
const handleUpdateValue = async(i, key, value) => {
  await proxy.$oui.call('uci', 'set', {
    config: 'firewall',
    section: `@rule[${i}]`,
    values: { [key]: value }
  })
}
const submit = async() => {
  const params = getUpdateParams()
  const arr = Object.keys(params)
  console.log(params)
  for (let i = 0; i < arr.length; i++) {
    if (arr[i] !== 'key') {
      await handleUpdateValue(params.key, arr[i], params[arr[i]])
    }
  }
  hidenDialog()
  getTableData()
}

// 获取列表数据
const getRules = async(i, key) => {
  const res = await proxy.$oui.call('uci', 'get', {
    config: 'firewall',
    section: `@rule[${i}]`,
    option: key
  })
  data.tableData[i][key] = res
}
const getTableData = async() => {
  for (let i = 0; i < data.tableData.length; i++) {
    const arr = Object.keys(data.tableData[i])
    for (let j = 0; j < arr.length; j++) {
      if (arr[j] !== 'key') {
        await getRules(i, arr[j])
      }
    }
  }
  data.loading = false
}
getTableData()

const { columns, tableData, showModal, model, loading } = toRefs(data)
</script>

<style scoped>
::v-deep .edit-btn {
  cursor: pointer;
}
</style>
