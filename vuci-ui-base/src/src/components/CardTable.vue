<template>
  <v-card>
    <v-card-title primary-title v-if="title">
      {{title}}
      <v-spacer></v-spacer>
      <v-text-field v-if="search" v-model="searched" append-icon="search" label="Search" single-line hide-details></v-text-field>
    </v-card-title>
    <v-divider></v-divider>
    <v-data-table :headers="headers" :items="items" :search="searched" :hide-actions="hideActions" :hide-headers="hideHeaders">
      <template slot="items" slot-scope="props">
        <td v-for="h in headers" :key="h.value">{{props.item[h.value]}}</td>
      </template>
    </v-data-table>
  </v-card>
</template>

<script>
  export default {
    name: 'card-table',
    props: {
      title: String,
      headers: Array,
      items: Array,
      hideActions: Boolean,
      hideHeaders: Boolean,
      search: {
        default: true,
        type: Boolean
      }
    },
    data() {
      return {
        searched: ''
      }
    }
  };
</script>

<style scoped>
  .v-card {
    margin-bottom: 20px;
  }
</style>
