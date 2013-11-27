<?php

/**
 * @file
 * Deploy Extra Log views field handlers.
 */

/**
 * Process status field from integer to string.
 */
class deploy_extra_log_handler_status_field extends views_handler_field {

  function render($values) {
    $value = _deploy_extra_log_status_to_string($values->{$this->field_alias});
    return $value;
  }

}