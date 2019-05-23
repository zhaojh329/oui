<template>
  <Card :title="title" style="margin-bottom: 15px">
    <Table :columns="columns" :data="data"></Table>
  </Card>
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