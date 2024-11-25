import os


def include_directory(
    install_path: str, source_path: str, exclude: list[str] = []
) -> list[tuple[str, list[str]]]:
    """
    Generates a list of tuples to include a whole directory into the data_files list.

    The list of tuples represents the directory structure and files within the given
    path, excluding specified file extensions.

    Each tuple in the list contains:
        - The path to a directory found within the traversed path.
        - A list of filenames within that directory, excluding those with
          extensions specified in the 'exclude' list.

    Args:
        install_path: The base path where files will be installed.
        source_path: The root directory containing the files to be included.
        exclude: A list of file extensions to exclude (e.g., [".txt", ".py"]).
            Defaults to an empty list.

    Returns:
        A list of tuples with the directory a list of files in that directory.
    """
    install_path = install_path.removesuffix(source_path)

    # Go through every nested sub-directory
    data_files = []
    for dirpath, dirnames, filenames in os.walk(source_path):
        # Filter files by the file extension
        files = [
            os.path.join(dirpath, file)
            for file in filenames
            if not any(file.endswith(file_extension) for file_extension in exclude)
        ]

        if len(files) >= 1:
            data_files.append((os.path.join(install_path, dirpath), files))

    return data_files
