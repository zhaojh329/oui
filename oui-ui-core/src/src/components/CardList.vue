<template>
  <Card :title="title">
    <Table :show-header="false" :columns="columns" :data="data"></Table>
  </Card>
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
      columns: [
        {key: 'name'},
        {key: 'value'}
      ],
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