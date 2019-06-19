<template>
  <el-card :header="title" style="margin-bottom: 15px">
    <el-table :data="data">
      <el-table-column v-for="item in columns" :key="item.key" :prop="item.key" :label="item.title"></el-table-column>
    </el-table>
  </el-card>
</template>

<script>
export default {
  name: 'CardTable',
  props: {
    title: String,
    columns: Array,
    data: Array
  }
}
</script>