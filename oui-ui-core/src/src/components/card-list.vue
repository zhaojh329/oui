<template>
  <el-card :header="title" style="margin-bottom: 15px">
    <el-table :show-header="false" :data="data">
      <el-table-column prop="name"></el-table-column>
      <el-table-column prop="value"></el-table-column>
    </el-table>
  </el-card>
</template>

<script>
export default {
  name: 'CardList',
  props: {
    title: String,
    list: Array
  },
  data() {
    return {
      data: []
    }
  },
  watch: {
    list() {
      this.data = this.list.map(e => {
        return {name: e[0], value: e[1]};
      });
    }
  }
}
</script>