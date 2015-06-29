// from execute.cpp
void DrawASCII(LWR& lwr) {
  string ascii_filename;
  fprintf(stdout, "What ASCII photo would you like me to draw?\n> ");
  cin >> ascii_filename;
  lwr.DrawASCIIPhoto(ASCII_PATH + ascii_filename + TXT);
}

// from LWR
  // Just for fun draw a file containing ASCII characters
  int DrawASCIIPhoto(std::string path_to_ascii_file = "",
    double speed_percent = 100.0);

// from utils
namespace AsciiArt {
  int Generate2DVectorFromASCIIFile(std::string ascii_file,
    std::vector<std::vector<char>>& ascii_img,
    std::unordered_map<char, std::string> character_set,
    std::vector<char>& char_appearance);
};

namespace AsciiArt {
  int Generate2DVectorFromASCIIFile(string ascii_file,
    vector<vector<char>>& ascii_img,
    unordered_map<char, string> character_set,
    vector<char>& char_appearance) {

    ifstream ss_ascii_file(ascii_file);
    if (!ss_ascii_file.is_open()) {
      printf("Unable to open ASCII file");
      return ERROR_FILE_NOT_FOUND;
    }

    string line;
    int row = 0;
    while (getline(ss_ascii_file, line)) {
      ascii_img.push_back(vector<char>());  // row vector columns set to line size
      for (unsigned int i = 0; i<line.length(); i++) {
        ascii_img[row].push_back(line.at(i));  // set character
        if (character_set.erase(line[i]) == 1) {  // just removed from map
          char_appearance.push_back(line.at(i));
        }
      }
      row++;
    }
    return SUCCESS;
  }
}


///////////////////////////////////////////////////////////////////////////////
////                                ASCII                                  ////
///////////////////////////////////////////////////////////////////////////////

// This comes from the thesis and thus needs to be heavily modified.


// .png or .jpg photo should first be converted into an ASCII photo.
/*
int LWR::DrawASCIIPhoto(string path_to_ascii_file,
  double speed_percent) {
  vector<vector<char>> ascii_img;
  vector<cart_motion*> character_motion_paths;
  unordered_map<char, int> character_freq;
  unsigned int err_val = 0;

  if (path_to_ascii_file != "") {
    // read ASCII photo from file into 2D vector.
    Generate2DVectorFromASCIIFile(path_to_ascii_file, ascii_img,
      character_freq);

    int img_width = ascii_img.at(0).size();
    int img_height = ascii_img.size();
    LWRLogParser log_parser = LWRLogParser();

    // load motion profiles for all characters in ascii drawing
    // from motion normalized log files
    for (const auto &myPair : character_freq) {
      string log_file = KUKA_LOG_DIR;
      log_file += myPair.first;
      log_file += "_standardized";
      log_file += KUKA_LOG_EXTENSION;
      log_parser.SetLogFile(log_file);

      character_motion_paths.push_back(new cart_motion());

      // parse log file and generate motion to repeat
      err_val = log_parser.ParseMotion(NULL, character_motion_paths.back());
      if (err_val != EOK) {
        printf("ERROR, could not parse input file");
        // add deleting
        // delete &cart_path;
        return ERROR_INVALID_INPUT_FILE;
      }
    }

  }
  else {
    // keyboard input script
  }
  return 1;
}
*/
int LWR::DrawASCIIPhoto(string path_to_ascii_file,
  double speed_percent) {
  // assume standardized character recording files have been created

  // start with map character -> filename
  string standardized_file_path = "C:\\Users\\HMMS\\Documents\\GitHub\\Thesis\\KUKA LWR\\logs\\standardized_char_set\\";
  string standardized_file_extension = ".dat";
  static unordered_map<char, string> char_filename_map = {
    { '$', "$" }, { ' ', "space" }, { '+', "+" }, { '7', "7" }, { '8', "8" },
    { ':', "colon" }, { '=', "equal" }, { '?', "question" }, { 'D', "D" },
    { 'I', "I" }, { 'M', "M" }, { 'N', "N" }, { 'O', "O" }, { 'Z', "Z" },
    { '~', "tilde" }, { '\n', "newline" }, {'\b', "newline_left"}
  };
  // reserve for char order and appearance in ASCII file
  vector<char> char_appearance;
  //float start[3] = { -0.7, 0.4, -0.0145 };
  float start[3] = { -0.70f, 0.0f, 0.200f };  // draw on table
  int column = 0;
  int row = 0;

  // create 2D character array for ascii art photo
  // get mapping in order of what characters are in the file
  vector<vector<char>> ascii_img;
  AsciiArt::Generate2DVectorFromASCIIFile(path_to_ascii_file, ascii_img, char_filename_map, char_appearance);

  // artificially add \n and \b to appearance
  char_appearance.push_back('\n');
  //char_appearance.push_back('\b');

  // map character to open file stream (hardcore)
  /*
  unordered_map<char, ifstream> char_files;
  for (int i = 0; i < char_appearance.size(); ++i) {
  char_files[i] = ifstream(char_filename_map[char_appearance[i]]);
  }
  */

  // load header information
  // assume char width, height, pose all the same for all standards
  int char_width;
  int char_height;
  float pose_rotation_matrix[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  Logging::Logger log;
  log.SetInputFile(standardized_file_path + char_appearance[0] + standardized_file_extension);
  log.ReadHeader();
  //LWRLogParser log_parser = LWRLogParser(standardized_file_path + char_appearance[0] + standardized_file_extension);
  //log_parser.ParseHeader();

  // get char width, char height, rotation matrix
  log.GetCharacterWidth(char_width);
  log.GetCharacterHeight(char_height);
  log.GetPoseRotationMatrix(pose_rotation_matrix);

  // load entire path for each used character
  // maps 'char' -> vector<float[3]> e.g. '$' -> [[1,2,3], [1,2,4], [1,2,2] ...
  unordered_map<char, vector<vector<float>>> char_path_map;
  for (unsigned int i = 0; i < char_appearance.size(); ++i) {
    char current_char = char_appearance[i];
    ifstream char_file = ifstream(standardized_file_path + char_filename_map[current_char] + standardized_file_extension);
    // add new entry to map for char
    char_path_map[current_char] = vector<vector<float>>();
    // read path
    log.ReadStandardizedCharacterPath(&char_file, char_path_map[current_char]);
  }

  // set rotation matrix in commanded pose
  // same rotation matrix is used for all points
  // limited scope
  {
    int i = 0;
    int j = 0;
    for (i; i < NUMBER_OF_FRAME_ELEMENTS; ++i) {
      pose_cmd[i] = pose_rotation_matrix[j++];
      if ((i + 1) % 4 == 0) {
        i++;
      }
    }
  }
  
  // get starting position
  char tmp = ascii_img[0][0];
  vector<float> starting_xyz = char_path_map[tmp][0];
  pose_cmd[3] = starting_xyz[0];
  pose_cmd[7] = starting_xyz[1];
  pose_cmd[11] = starting_xyz[2];
  // add starting position offset

  // move to starting position
  int err_val = EOK;
  printf("MOVING to starting position.\n");
  
  //err_val = MoveToJointPosition({-0.669f, 43.977f, 0.690f, -101.919f, -0.207f, -55.887f, -0.368f});
  //err_val = MoveToCartesianPose(pose_cmd);
  //if (err_val != EOK) {
  //  fri_->printf("ERROR, cannot move robot to position\n");
  //}
  //else {
  //  printf("MOVED to starting position\n");
  //}
  // start cartesian impedance control mode
  err_val = StartCartesianImpedanceControlMode(CARTESIAN_STIFFNESS_HIGH, CARTESIAN_DAMPING_LOW, CARTESIAN_TORQUE_NONE);
  if (err_val != EOK) {
    printf("ERROR, could not start robot in CartImpedanceControlMode");
    return err_val;
  }

  ofstream output("C:\\Users\\HMMS\\Documents\\GitHub\\Thesis\\KUKA LWR\\misc\\ascii_art_path.txt");

  // loop over char in 2D ascii art array

  // for this printer pattern add:
  //    new line at end to even line numbers
  //    inv(space), new line to the start odd line numbers
  /*
  for (int i = 0; i < ascii_img.size(); ++i) {
    if (i % 2 == 0) {
      ascii_img[i].push_back('\n');
    }
    else {
      ascii_img[i].insert(ascii_img[i].begin(), '\b');
    }
  }
  */
  row = 0;
  for (auto ascii_row : ascii_img) {
    column = 0;
    if ((row % 2) == 1) { // odd == right to left

      for (column = ascii_row.size() - 1; column >= 0; column--) {
        char ascii_character = ascii_row[column];
        printf("%c", ascii_character);
        //output << ascii_character << "\n";
        auto path = char_path_map[ascii_character];
        for (int elem = path.size() -1 ; elem >= 0; elem--) {
          //printf("%d/%d\n", path.size()-1-elem, path.size());
          // wait one KRC iteration cycle
          fri_->WaitForKRCTick();

          // check for connection issues
          
          if (!fri_->IsMachineOK()) {
          fprintf(stderr, "ERROR, the machine is not ready anymore.\n");
          err_val = Errors::ERROR_MACHINE_NOT_OKAY;
          break;
          }
          

          // set commanded pose displacements to standardized path value
          // rotations will never change and are set origionally
          pose_cmd[3] = path[elem][0];
          pose_cmd[7] = path[elem][1];
          pose_cmd[11] = path[elem][2];

          // add starting point
          pose_cmd[3] += start[0];
          pose_cmd[7] += start[1];
          pose_cmd[11] += start[2];

          // add position x y global offset
          pose_cmd[3] += (float)(column*char_width) / 1000.0f;  // mm to m
          pose_cmd[7] -= (float)(row*char_height) / 1000.0f;

          fri_->SetCommandedCartPose(pose_cmd);
          //printf("%.5f, %.5f, %.5f\n", pose_cmd[3], pose_cmd[7], pose_cmd[11]);
          output << pose_cmd[3] << "\t" << pose_cmd[7] << "\t" << pose_cmd[11] << "\n";
        }
      }
      printf("\n");
    }
    else {  // forward
      for (column = 0; column < static_cast<int>(ascii_row.size()); ++column) {
        char ascii_character = ascii_row[column];
        printf("%c", ascii_character);
        //output << ascii_character << "\n";
        auto path = char_path_map[ascii_character];

        for (unsigned int elem = 0; elem < path.size(); ++elem) {
          //printf("%d/%d\n", elem, path.size()-1);
          // wait one KRC iteration cycle
          fri_->WaitForKRCTick();

          // check for connection issues
          
          if (!fri_->IsMachineOK()) {
          fprintf(stderr, "ERROR, the machine is not ready anymore.\n");
          err_val = Errors::ERROR_MACHINE_NOT_OKAY;
          break;
          }
          

          // set commanded pose displacements to standardized path value
          // rotations will never change and are set origionally
          pose_cmd[3] = path[elem][0];
          pose_cmd[7] = path[elem][1];
          pose_cmd[11] = path[elem][2];

          // add starting point
          pose_cmd[3] += start[0];
          pose_cmd[7] += start[1];
          pose_cmd[11] += start[2];

          // add position x y global offset
          pose_cmd[3] += (float)(column*char_width) / 1000.0f;  // mm to m
          pose_cmd[7] -= (float)(row*char_height) / 1000.0f;

          fri_->SetCommandedCartPose(pose_cmd);
          //printf("%.5f, %.5f, %.5f\n", pose_cmd[3], pose_cmd[7], pose_cmd[11]);
          output << pose_cmd[3] << "\t" << pose_cmd[7] << "\t" << pose_cmd[11] << "\n";
        }
      }
      printf("\n");
    }
    // add new line
    auto path = char_path_map['\n'];
    column < 0 ? column++ : column-- ;
    for (unsigned int elem = 0; elem < path.size(); ++elem) {
      //printf("%d/%d\n", elem, path.size() - 1);
      // wait one KRC iteration cycle
      fri_->WaitForKRCTick();

      // check for connection issues

      if (!fri_->IsMachineOK()) {
        fprintf(stderr, "ERROR, the machine is not ready anymore.\n");
        err_val = Errors::ERROR_MACHINE_NOT_OKAY;
        break;
      }

      // set commanded pose displacements to standardized path value
      // rotations will never change and are set origionally
      pose_cmd[3] = path[elem][0];
      pose_cmd[7] = path[elem][1];
      pose_cmd[11] = path[elem][2];

      // add starting point
      pose_cmd[3] += start[0];
      pose_cmd[7] += start[1];
      pose_cmd[11] += start[2];

      // add position x y global offset
      pose_cmd[3] += (float)(column*char_width) / 1000.0f;  // mm to m
      pose_cmd[7] -= (float)(row*char_height) / 1000.0f;

      fri_->SetCommandedCartPose(pose_cmd);
      //printf("%.5f, %.5f, %.5f\n", pose_cmd[3], pose_cmd[7], pose_cmd[11]);
      output << pose_cmd[3] << "\t" << pose_cmd[7] << "\t" << pose_cmd[11] << "\n";
    }

    row++;
  }

  // stop
  fri_->StopRobot();
  return EOK;
}