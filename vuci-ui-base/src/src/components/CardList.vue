<template>
  <card-table :title="title" :headers="headers" :items="items" hide-actions hide-headers :search="false"></card-table>
</template>

<script>
  export default {
    name: 'card-list',
    props: {
      title: String,
      list: Array
    },
    data() {
      return {
        headers: [
          {value: 'name', sortable: false},
          {value: 'value', sortable: false}
        ],
        items: []
      }
    },
    watch: {
      list() {
        this.items = this.list.map(item => {
          return {name: item[0], value: item[1]};
        });
      }
    }
  };
</script>
